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

#include "lang/nfa/state.hpp"
#include "lang/nfa/fragment.hpp"
#include "core/core.hpp"

#include <string>

namespace lang
{
    //! A regular definition entry (possibly referring
    //!   to other regular definitions)
    struct RegularDefinition
    {
        //! String representation of this regular definition
        std::string as_string;
        //! Compiled NFA fragment associated with this definition
        nfa::Fragment fragment;
        //! Start state of the definition's NFA
        //! Non-null only for top-level definitions
        nfa::State* start;
        //! Priority of this definiton, the lowest this value
        //!   is, more this definition is strong
        int priority;
        //! Functor object used to build the associated token
        //!   from the extracted lexeme.
        //! nil for non top-level definitions
        core::Object build_token;
    };
}

#endif // __AXOLOTL_LANG_REGULAR_DEFINITION_H__
