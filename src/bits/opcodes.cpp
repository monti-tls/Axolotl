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

#include "bits/opcodes.hpp"

namespace bits
{
    std::string opcode_as_string(Opcode op)
    {
        struct Pair
        {
            Opcode op;
            const char* name;
        };

        static Pair names[] =
        {
            #define OPCODE(name, nargs) { name, #name },
            #include "bits/opcodes.def"
            #undef OPCODE
        };

        for (int i = 0; i < (int) (sizeof(names) / sizeof(Pair)); ++i)
        {
            if (op == names[i].op)
                return std::string(names[i].name);
        }

        return std::string("<INVALID>");
    }
}