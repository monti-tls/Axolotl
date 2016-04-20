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

#ifndef __AXOLOTL_LANG_REGULAR_DEFINITION_COMPILER_H__
#define __AXOLOTL_LANG_REGULAR_DEFINITION_COMPILER_H__

#include "lang/nfa_fragment.hpp"
#include "lang/regular_definition.hpp"
#include "core/core.hpp"

#include <string>
#include <iostream>
#include <map>

namespace lang
{
    class RegularDefinitionCompiler
    {
    private:
        struct Token
        {
            enum
            {
                EOL,
                STRING,
                CHAR,
                CHAR_CLASS,
                DOT,
                LPAREN,
                RPAREN,
                STAR,
                PLUS,
                QUESTION_MARK,
                PIPE,
                IDENTIFIER
            } which;

            core::Some what;
            int line, col;
        };

    public:
        RegularDefinitionCompiler(std::istream& in, std::map<std::string, RegularDefinition> const& defs);

        void rewind();
        nfa::Fragment compile();

    private:
        int M_nextChar() const;
        int M_getChar();
        void M_eatChar(int check);
        void M_skipSpaces();
        Token const& M_nextToken();
        Token M_getToken();
        char M_escapeSequence();
        void M_eatToken(int check);
        void M_lexerError(std::string const& msg);
        nfa::Fragment M_alternation();
        nfa::Fragment M_concatenation();
        nfa::Fragment M_repetition();
        nfa::Fragment M_atom();
        void M_parserError(Token const& tok, std::string const& msg);

    private:
        std::istream& m_in;
        std::map<std::string, RegularDefinition> const& m_defs;
        int m_line, m_col, m_pos;
        int m_next_char;
        Token m_next_token;
    };
}

#endif // __AXOLOTL_LANG_REGULAR_DEFINITION_COMPILER_H__
