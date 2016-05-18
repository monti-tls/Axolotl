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

#ifndef __AXOLOTL_BITS_OPCODES_H__
#define __AXOLOTL_BITS_OPCODES_H__

#include <string>

namespace bits
{
    enum Opcode
    {
        #define DEF_MASK(name, value)
        #define DEF_OPCODE(name, nargs) name,
        #include "bits/opcodes.def"
        #undef DEF_OPCODE
        #undef DEF_MASK


        #define DEF_MASK(name, value) name ## _MASK = value,
        #define DEF_OPCODE(name, nargs)
        #include "bits/opcodes.def"
        #undef DEF_OPCODE
        #undef DEF_MASK
    };

    std::string opcode_as_string(Opcode op);
    int opcode_nargs(Opcode op);
    Opcode opcode_remove_masks(int op);
}

#endif // __AXOLOTL_BITS_OPCODES_H__
