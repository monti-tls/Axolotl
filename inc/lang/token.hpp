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

#ifndef __AXOLOTL_LANG_TOKEN_H__
#define __AXOLOTL_LANG_TOKEN_H__

#include "core/core.hpp"

#include <string>

namespace lang
{
    class Token
    {
    public:
        enum
        {
            Invalid = -3,
            Eof     = -2,
            Skip    = -1
        };

    public:
        struct Where
        {
            std::size_t line = 1UL;
            std::size_t col = 0UL;
            std::string filename = "";
        };

    public:
        Token(int which = Invalid, core::Object const& what = core::Object::nil());

        int which() const;
        core::Object const& what() const;

        Where const& where() const;
        void setWhere(Where const& where);

        std::string const& lexeme() const;
        void setLexeme(std::string const& lexeme);

    private:
        int m_which;
        core::Object m_what;
        Where m_where;
        std::string m_lexeme;
    };
}

#endif // __AXOLOTL_LANG_TOKEN_H__
