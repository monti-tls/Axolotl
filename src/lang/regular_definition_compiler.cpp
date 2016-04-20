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

#include "lang/regular_definition_compiler.hpp"

#include <sstream>
#include <stdexcept>

using namespace lang;
using namespace nfa;

RegularDefinitionCompiler::RegularDefinitionCompiler(std::istream& in, std::map<std::string, RegularDefinition> const& defs)
    : m_in(in)
    , m_defs(defs)
{ rewind(); }

void RegularDefinitionCompiler::rewind()
{
    m_in.clear();
    m_in.seekg(0);
    m_line = 1;
    m_col = 0;
    m_next_char = 0;
    M_getChar();
    M_getToken();
}

Fragment RegularDefinitionCompiler::compile()
{ return M_alternation(); }

int RegularDefinitionCompiler::M_nextChar() const
{ return m_next_char; }

int RegularDefinitionCompiler::M_getChar()
{
    int la = m_next_char;

    if (la == '\n')
    {
        ++m_line;
        m_col = 1;
    }
    else
        ++m_col;

    m_next_char = m_in.get();
    if (!m_in)
        m_next_char = 0;

    return la;
}

void RegularDefinitionCompiler::M_eatChar(int check)
{
    int got = M_getChar();

    if (check && got != check)
    {
        std::ostringstream ss;
        ss << "expected '" << (char) check << "'";
        M_lexerError(ss.str());
    }
}

void RegularDefinitionCompiler::M_skipSpaces()
{
    while (std::isspace(M_nextChar()))
        M_getChar();
}

RegularDefinitionCompiler::Token const& RegularDefinitionCompiler::M_nextToken()
{ return m_next_token; }

RegularDefinitionCompiler::Token RegularDefinitionCompiler::M_getToken()
{
    Token tok = m_next_token;

    m_next_token.what.clear();
    m_next_token.line = m_line;
    m_next_token.col = m_col;

    M_skipSpaces();
    switch (M_nextChar())
    {
        case 0:
            m_next_token.which = Token::EOL;
            break;

        case '\'': // CHAR
        {
            M_eatChar('\'');

            char c;
            switch (M_nextChar())
            {
                case '\\':
                    c = M_escapeSequence();
                    break;

                case '[':
                {
                    M_eatChar('[');

                    bool invert = false;
                    if (M_nextChar() == '^')
                    {
                        M_eatChar('^');
                        invert = true;
                    }

                    std::vector<std::pair<char, char>> ranges;
                    while (M_nextChar() != ']')
                    {
                        char low;
                        if (M_nextChar() == '\\')
                            low = M_escapeSequence();
                        else
                            low = M_getChar();

                        char high = low;
                        if (M_nextChar() == '-')
                        {
                            M_getChar();

                            if (M_nextChar() == '\\')
                                high = M_escapeSequence();
                            else
                                high = M_getChar();
                        }

                        ranges.push_back(std::make_pair(low, high));
                    }

                    M_eatChar(']');
                    M_eatChar('\'');

                    m_next_token.which = Token::CHAR_CLASS;
                    m_next_token.what = std::make_pair(invert, ranges);
                    goto break_outer;
                }

                default:
                    c = M_getChar();
                    break;
            }

            M_eatChar('\'');

            m_next_token.which = Token::CHAR;
            m_next_token.what = c;

            break_outer:
            break;
        }

        case '"': // STRING
        {
            M_eatChar('"');

            std::string s;

            while (M_nextChar() != '"')
            {
                char c;

                if (M_nextChar() == '\\')
                    c = M_escapeSequence();
                else
                    c = M_getChar();

                s += c;
            }
            M_eatChar('"');

            m_next_token.which = Token::STRING;
            m_next_token.what = s;
            break;
        }

        case '.': // DOT
            M_getChar();
            m_next_token.which = Token::DOT;
            break;

        case '(': // LPAREN
            M_getChar();
            m_next_token.which = Token::LPAREN;
            break;

        case ')': // RPAREN
            M_getChar();
            m_next_token.which = Token::RPAREN;
            break;

        case '*': // STAR
            M_getChar();
            m_next_token.which = Token::STAR;
            break;

        case '+': // PLUS
            M_getChar();
            m_next_token.which = Token::PLUS;
            break;
        
        case '?': // QUESTION_MARK
            M_getChar();
            m_next_token.which = Token::QUESTION_MARK;
            break;

        case '|': // PIPE
            M_getChar();
            m_next_token.which = Token::PIPE;
            break;

        default: // IDENTIFIER
        {
            std::string s;

            bool first_letter = true;

            while (std::isalpha(M_nextChar()) ||
                   M_nextChar() == '_' ||
                   (!first_letter && std::isdigit(M_nextChar())))
            {
                s += M_getChar();
                first_letter = false;
            }

            if (first_letter)
                M_lexerError("unexpected character");

            m_next_token.which = Token::IDENTIFIER;
            m_next_token.what = s;
            break;
        }
    }

    return tok;
}

char RegularDefinitionCompiler::M_escapeSequence()
{
    char c = M_getChar();
    if (c != '\\')
        throw std::runtime_error("lang::RegularDefinitionCompiler::M_escapeSequence: not an escace sequence");

    c = M_getChar();
    switch (c)
    {
        case '\\':
            c = '\\';
            break;

        case '\'':
            c = '\'';
            break;

        case '"':
            c = '"';
            break;

        case ']':
            c = ']';
            break;

        case '^':
            c = '^';

        case '-':
            c = '-';

        default:
            M_lexerError("undefined escape sequence");
            break;
    }

    return c;
}

void RegularDefinitionCompiler::M_eatToken(int check)
{
    Token tok = M_getToken();
    if (tok.which != check)
        M_parserError(tok, "unexpected token");
}

void RegularDefinitionCompiler::M_lexerError(std::string const& msg)
{
    std::ostringstream ss;
    ss << "RegularDefinitionCompiler::M_lexerError: ";
    ss << "line " << m_line << ", col " << m_col << ": " << msg << std::endl;
    throw std::runtime_error(ss.str());
}

Fragment RegularDefinitionCompiler::M_alternation()
{
    Fragment frag = M_concatenation();

    while (M_nextToken().which == Token::PIPE)
    {
        M_getToken();
        frag = Fragment::alternate(frag, M_concatenation());
    }

    return frag;
}

Fragment RegularDefinitionCompiler::M_concatenation()
{
    Fragment frag = M_repetition();

    while (M_nextToken().which != Token::EOL &&
           M_nextToken().which != Token::PIPE &&
           M_nextToken().which != Token::RPAREN)
    {
        frag = Fragment::concatenate(frag, M_repetition());
    }

    return frag;
}

Fragment RegularDefinitionCompiler::M_repetition()
{
    Fragment frag = M_atom();

    switch (M_nextToken().which)
    {
        case Token::QUESTION_MARK:
        {
            M_getToken();
            frag = Fragment::optional(frag);
            break;
        }

        case Token::STAR:
        {
            M_getToken();
            frag = Fragment::loop(frag);
            break;
        }

        case Token::PLUS:
        {
            M_getToken();
            frag = Fragment::loopone(frag);
            break;
        }

        default:
            break;
    }

    return frag;
}

Fragment RegularDefinitionCompiler::M_atom()
{
    switch (M_nextToken().which)
    {
        case Token::IDENTIFIER:
        {
            Token tok = M_getToken();
            std::string id = tok.what.as<std::string>();
            auto it = m_defs.find(id);

            if (it == m_defs.end())
                M_parserError(tok, "use of unknown identifier '" + id + "'");
            if (it->second.start)
                M_parserError(tok, "use of top-level definition '" + id + "'");

            std::istringstream ss;
            ss.str(it->second.as_string);
            RegularDefinitionCompiler rxc(ss, m_defs);

            return rxc.compile();
        }

        case Token::STRING:
        {
            Token tok = M_getToken();
            return Fragment::string(tok.what.as<std::string>());
        }

        case Token::CHAR:
        {
            Token tok = M_getToken();
            return Fragment::match(tok.what.as<char>());
        }

        case Token::CHAR_CLASS:
        {
            Token tok = M_getToken();
            std::pair<bool, std::vector<std::pair<char, char>>> cls = tok.what.as<decltype(cls)>();

            if (!cls.second.size())
                M_parserError(tok, "empty character class");

            return Fragment::charClass(cls);
        }

        case Token::DOT:
        {
            M_getToken();
            return Fragment::wildcard();
        }

        case Token::LPAREN:
        {
            M_getToken();
            Fragment frag = M_alternation();
            M_eatToken(Token::RPAREN);
            return frag;
        }

        default:
            M_parserError(M_nextToken(), "unexpected token");
            break;
    }

    return Fragment(); // never reached
}

void RegularDefinitionCompiler::M_parserError(Token const& tok, std::string const& msg)
{
    std::ostringstream ss;
    ss << "RegularDefinitionCompiler::M_parserError: ";
    ss << "line " << tok.line << ", col " << tok.col << ": " << msg << std::endl;
    throw std::runtime_error(ss.str());
}
