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

#ifndef __AXOLOTL_LANG_PARSER_H__
#define __AXOLOTL_LANG_PARSER_H__

#include "lang/lexer.hpp"

#include <string>
#include <iostream>
#include <map>

namespace lang
{
    class Parser
    {
    public:
        Parser(std::istream& in, std::size_t lookhead = 1, std::size_t lex_lookahead = 128);
        virtual ~Parser();

        void rewind();

        std::string message(Token const& token, std::string const& msg);
        void error(Token const& token, std::string const& msg);
        void warning(Token const& token, std::string const& msg);

    protected:
        virtual void M_setupLexer() = 0;
        virtual void M_setupTokens() = 0;

        void M_build();
        void M_define(std::string const& name, std::string const& definition, core::Object const& build_token);
        void M_setTokenName(int token, std::string const& name);
        std::string M_tokenName(int token);
        void M_initLookahead();
        Token M_get();
        Token M_eat(int which);
        Token const& M_peek(std::size_t depth = 0);
        void M_unexpected(Token const& token);

    private:
        Lexer m_lexer;
        std::map<int, std::string> m_token_names;
        std::size_t m_lookahead_depth;
        std::vector<Token> m_lookahead;
    };
}

#endif // __AXOLOTL_LANG_PARSER_H__
