/*  This file is part of Axolotl.
 *
 * Axolotl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Axolotl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Axolotl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lang/lexer.hpp"
#include "lang/regular_definition_compiler.hpp"
#include "core/core.hpp"

#include <sstream>
#include <stack>
#include <stdexcept>
#include <set>

using namespace lang;
using namespace nfa;
using namespace core;

Lexer::Lexer(std::istream& in, std::size_t buffer_size)
    : m_in(in)
    , m_buffer_size(buffer_size)
    , m_defs_priority(0)
    , m_nfa_start(nullptr)
{
    M_allocateBuffers();
    rewind();
}

Lexer::~Lexer()
{
    M_releaseNfa();
    M_releaseBuffers();
}

void Lexer::rewind()
{
    m_in.clear();
    m_in.seekg(0);

    m_eof = false;
    M_initPointers();
    M_loadBuffer(m_right);
}

void Lexer::addDefinition(std::string const& name, std::string const& definition, Object const& build_token)
{
    if (m_defs.find(name) != m_defs.end())
        throw std::runtime_error("lang::Lexer::addDefinition: redefining `" + name + "'");

    std::istringstream ss;
    ss.str(definition);

    RegularDefinitionCompiler compiler(ss, m_defs);
    RegularDefinition& def = m_defs[name];

    def.as_string = definition;
    def.start = nullptr;
    if (!build_token.isNil())
    {
        def.fragment = compiler.compile();

        MatchState* match = new MatchState();
        match->what = State::Match;
        match->defname = name;
        def.fragment.patch(match);

        def.start = def.fragment.start();
        def.priority = ++m_defs_priority;
        def.build_token = build_token;
    }
}

void Lexer::build()
{
    if (!m_defs.size())
        throw std::runtime_error("lang::Lexer::build: there are no definitions");

    if (m_nfa_start)
        throw std::runtime_error("lang::Lexer::build: NFA is already built");

    Fragment frag;
    bool first = true;

    for (auto def : m_defs)
    {
        if (!def.second.start)
            continue;

        if (first)
        {
            first = false;
            frag = def.second.fragment;
        }
        else
            frag = Fragment::alternate(frag, def.second.fragment);
    }

    if (first)
        throw std::runtime_error("lang::Lexer::build: there are no top-level definitions");

    m_nfa_start = frag.start();
}

Token Lexer::M_getToken()
{
    if (!m_nfa_start)
        throw std::runtime_error("lang::Lexer::M_getToken: lang::Lexer::build() must be called before using the parser");

    // If the EOF bit is set, don't do anything
    if (m_eof)
        return Token(Token::Eof);

    //! This structure holds a match state
    struct Match
    {
        MatchState* state;
        char* forward;
        std::size_t size;
        int priority;
    };

    // We will put eventual matches here
    std::vector<Match> match_list;
    // And here are our state lists
    std::vector<State*> state_list, next_state_list;

    // Enter into the initial state
    M_addStateInList(m_nfa_start, state_list);

    // Simulate our NFA
    for (std::size_t size = 0; ; ++size)
    {
        // Get the next character to examine
        char c = M_advanceForward();

        for (auto state : state_list)
        {
            // If we encounter a match state, put it into our list
            //   and save information about where it happenned, when and
            //   to which definition it is related
            if (state->what == State::Match)
            {
                Match match;
                match.state = (MatchState*) state;
                match.forward = m_forward;
                match.size = size;
                match.priority = m_defs[match.state->defname].priority;

                match_list.push_back(match);
            }
            else if (state->what == State::Wildcard)
            {
                M_addStateInList(state->out[0], next_state_list);
            }
            else if (state->what == State::Range)
            {
                bool in_class = false;
                for (CharClass* cls = state->char_class; cls; cls = cls->next)
                {
                    in_class = in_class || (c >= cls->low && c <= cls->high);
                    if (in_class)
                        break;
                }

                if (state->invert)
                    in_class = !in_class;

                if (in_class)
                    M_addStateInList(state->out[0], next_state_list);
            }
        }

        // Exit if we reached EOF or if we don't have
        //   no more states in our list
        if (!next_state_list.size() || m_eof)
            break;

        // Otherwise, swap and clear
        state_list = next_state_list;
        next_state_list.clear();
    }

    if (!match_list.size())
    {
        M_resetBegin();
        return Token(Token::Invalid);
    }

    // Search for the deepest match
    std::size_t max_size = 0;
    for (auto m : match_list)
    {
        if (m.size > max_size)
            max_size = m.size;
    }

    // Search for the highest priority (lowest number)
    //   between the deppest matches
    int max_priority = -1;
    for (auto m : match_list)
    {
        if (m.size < max_size)
            continue;

        if (max_priority < 0 || m.priority < max_priority)
            max_priority = m.priority;
    }

    // Choose the final match based on a longest prefix, highest priority criteria
    for (auto m : match_list)
    {
        if (m.size == max_size &&
            m.priority == max_priority)
        {
            // Reset our forward pointer so we can process further tokens
            m_forward = m.forward;
            if (*m_forward)
                M_retractForward();

            // Setup the token we return
            Object obj = m_defs[m.state->defname].build_token(M_getLexeme());
            if (!obj.meta().is<Token>())
                throw std::runtime_error("lang::Lexer::M_getToken: invalid token object");
            Token tok = obj.unwrap<Token>();

            // Reset our begin pointer so that it points
            //   to the beginning of the next lexeme
            M_resetBegin();

            return tok;
        }
    }

    throw std::runtime_error("lang::Lexer::M_getToken: internal error (priority mess)");
}

void Lexer::M_allocateBuffers()
{
    m_buffers[0] = new char[m_buffer_size+1];
    m_buffers[1] = new char[m_buffer_size+1];
}

void Lexer::M_releaseBuffers()
{
    delete[] m_buffers[0];
    delete[] m_buffers[1];
}

void Lexer::M_loadBuffer(std::size_t id)
{
    if (id >= 2)
        throw std::runtime_error("lang::Lexer::M_loadBuffer: invalid buffer index");

    m_in.read(m_buffers[id], m_buffer_size);
    if (!m_in)
        m_buffers[id][m_in.gcount()] = 0;
    else
        m_buffers[id][m_buffer_size] = 0;
}

void Lexer::M_initPointers()
{
    m_left = 0;
    m_right = 1;
    m_forward = m_buffers[m_right] - 1;
    m_begin = m_forward + 1;
}

void Lexer::M_resetBegin()
{
    if (m_eof)
        return;

    m_begin = m_forward + 1;
    
    if (!*m_begin)
    {
        if (m_begin == m_buffers[m_left] + m_buffer_size)
            m_begin = m_buffers[m_right];
        else if (m_begin == m_buffers[m_right] + m_buffer_size)
            throw std::runtime_error("lang::Lexer::M_resetBegin: internal error");
        else
            m_eof = true;
    }
}

char Lexer::M_advanceForward()
{
    if (m_eof)
        return 0;

    ++m_forward;

    if (!*m_forward)
    {
        // Case where F was resetted inside (L) buffer,
        //   but the (R) buffer was already loaded
        if (m_forward == m_buffers[m_left] + m_buffer_size)
        {
            m_forward = m_buffers[m_right];
        }
        // Case where F hits the end of the (R) buffer and we
        //   need to re-load the left buffer in order to keep going
        else if (m_forward == m_buffers[m_right] + m_buffer_size)
        {
            // If B is still in (L) buffer we fail because reloading
            //   it would invalid our B pointer
            if (m_begin >= m_buffers[m_left] &&
                m_begin <= m_buffers[m_left] + m_buffer_size)
                throw std::runtime_error("lang::Lexer::M_advanceForward: lookahead limit reached");

            // Load the next buffer into (L)
            M_loadBuffer(m_left);

            // Swap (L) and (R) designations
            std::swap(m_left, m_right);

            // Set F to (R) begin
            m_forward = m_buffers[m_right];
        }
    }
            
    if (!*m_forward)
    {
        m_eof = true;
        return 0;
    }

    return *m_forward;
}

void Lexer::M_retractForward()
{
    if (m_eof)
        return;

    if (m_forward == m_buffers[m_right])
        m_forward = m_buffers[m_left] + m_buffer_size - 1;
    else if (m_forward == m_buffers[m_left])
        throw std::runtime_error("lang::Lexer::M_retractForward: can't retract past the left buffer");
    else
        --m_forward;
}

std::string Lexer::M_getLexeme()
{
    std::string lexeme;
    char* it = m_begin;

    for (; *it; ++it)
    {
        lexeme += *it;
        if (it == m_forward)
            return lexeme;
    }

    if (it == m_buffers[m_left] + m_buffer_size)
        it = m_buffers[m_right];

    if (it == m_forward)
        return lexeme;

    for (; *it; ++it)
    {
        lexeme += *it;
        if (it == m_forward)
            return lexeme;
    }

    return lexeme;
}

void Lexer::M_addStateInList(State* state, std::vector<State*>& state_list)
{
    std::stack<State*> stack;
    stack.push(state);

    while (stack.size())
    {
        State* state = stack.top();
        stack.pop();

        if (state->what == State::Split)
        {
            if (state->out[0])
                stack.push(state->out[0]);
            if (state->out[1])
                stack.push(state->out[1]);
        }
        else
            state_list.push_back(state);
    }
}

void Lexer::M_releaseNfa()
{
    if (!m_nfa_start)
        return;

    std::set<State*> all_states;
    std::stack<State*> stack;
    stack.push(m_nfa_start);

    while (stack.size())
    {
        State* state = stack.top();
        stack.pop();

        all_states.emplace(state);

        if (state->out[0] && all_states.find(state->out[0]) == all_states.end())
            stack.push(state->out[0]);
        if (state->out[1] && all_states.find(state->out[1]) == all_states.end())
            stack.push(state->out[1]);
    }

    for (auto s : all_states)
    {
        for (CharClass* cls = s->char_class; cls; )
        {
            CharClass* next = cls->next;
            delete cls;
            cls = next;
        }

        delete s;
    }
}