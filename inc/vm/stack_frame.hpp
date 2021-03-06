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

#ifndef __AXOLOTL_VM_STACK_FRAME_H__
#define __AXOLOTL_VM_STACK_FRAME_H__

#include "core/some.hpp"
#include "vm/forward.hpp"
#include "vm/module.hpp"

namespace vm
{
    struct StackFrame
    {
        bool dummy;
        int pc;
        Module module;
        int locals_start;
        int locals_count;
        int argc;
        core::Some debug;
    };
}

#endif // __AXOLOTL_VM_STACK_FRAME_H__
