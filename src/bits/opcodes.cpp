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

#include <map>

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

    int opcode_nargs(Opcode op)
    {
        static std::map<Opcode, int> opcodes_nargs;
        static bool opcodes_nargs_inited = false;

        if (!opcodes_nargs_inited)
        {   
            #define OPCODE(name, nargs) opcodes_nargs[name] = nargs;
            #include "bits/opcodes.def"
            #undef OPCODE

            opcodes_nargs_inited = true;
        }

        auto it = opcodes_nargs.find(op);
        if (it == opcodes_nargs.end())
            return -1;

        return it->second;
    }
}
