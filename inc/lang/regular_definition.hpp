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

#ifndef __AXOLOTL_LANG_REGULAR_DEFINITION_H__
#define __AXOLOTL_LANG_REGULAR_DEFINITION_H__

#include "nfa_state.hpp"
#include "nfa_fragment.hpp"

namespace lang
{
    struct RegularDefinition
    {
        nfa::Fragment fragment;

        //! Start state of the definition's NFA
        //! Non-null only for top-level definitions
        nfa::State* start;

        //! Priority of this definiton, the lowest this value
        //!   is, more this definition is strong
        int priority;
    };
}

#endif // __AXOLOTL_LANG_REGULAR_DEFINITION_H__
