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

#ifndef __AXOLOTL_LANG_LEXER_H__
#define __AXOLOTL_LANG_LEXER_H__

#include "lang/nfa/state.hpp"
#include "lang/regular_definition.hpp"
#include "lang/token.hpp"
#include "core/core.hpp"

#include <string>
#include <iostream>
#include <map>

namespace lang
{
    class Lexer
    {
    public:
        Lexer(std::istream& in, std::size_t buffer_size);
        ~Lexer();

        void rewind();
        void define(std::string const& name, std::string const& definition, core::Object const& build_token = core::Object::nil());
        void build();
        Token getToken();
        bool eof() const;
        std::string snippet(Token const& token, std::size_t& pos);
        static std::string snippet(std::istream& is, std::size_t line, std::size_t col, std::size_t& pos);
        std::string streamName() const;

    private:
        void M_allocateBuffers();
        void M_releaseBuffers();
        void M_loadBuffer(std::size_t id);
        void M_initPointers();
        void M_resetBegin();
        char M_advanceForward();
        void M_retractForward();
        std::string M_getLexeme();
        void M_addStateInList(nfa::State* state, std::vector<nfa::State*>& state_list);
        void M_releaseNfa();
        Token M_setupToken(Token const& token);

    private:
        std::istream& m_in;

        char* m_buffers[2];
        std::size_t m_buffer_size;
        std::size_t m_left;
        std::size_t m_right;

        Token::Where m_where;

        char* m_forward;
        char* m_begin;
        bool m_eof;

        int m_defs_priority;
        std::map<std::string, RegularDefinition> m_defs;
        nfa::State* m_nfa_start;
    };
}

#endif // __AXOLOTL_LANG_LEXER_H__
