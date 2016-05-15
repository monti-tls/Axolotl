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

#ifndef __AXOLOTL_LANG_COMPILER_H__
#define __AXOLOTL_LANG_COMPILER_H__

#include "lang/forward.hpp"

#include <iostream>

namespace lang
{
    class Compiler
    {
    public:
        Compiler(std::istream& in);
        ~Compiler();

        void compile();
        void prettyPrint(std::ostream& os);

    private:
        void M_parse();
        void M_pass();

    private:
        std::istream& m_in;
        Parser* m_parser;
        ast::Node* m_root;
    };
}

#endif // __AXOLOTL_LANG_COMPILER_H__
