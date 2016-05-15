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

#ifndef __AXOLOTL_LANG_NFA_STATE_H__
#define __AXOLOTL_LANG_NFA_STATE_H__

#include <string>

namespace lang
{
    namespace nfa
    {
        //! A character class as a linked list of character ranges
        struct CharClass
        {
            //! The included lower range bound
            int low;
            //! The included higher range bound
            int high;
            //! Pointer to the eventual next range in the class
            CharClass* next = nullptr;
        };

        //! A single NFA state
        struct State
        {
            //! Kind of the NFA state
            enum Kind
            {
                //! The state matches any character
                Wildcard,
                //! The state matches a character class
                Range,
                //! The state is a 'split' state and is followed
                //!   unconditionnally
                Split,
                //! The state is a match state
                Match
            } what;

            //! The matched character class (may consist of a single
            //!   character range if a single character is matched)
            CharClass* char_class = nullptr;
            //! Whether or not the character class is inverted
            bool invert = false;
            //! Outgoing transitions (one or both may be 0)
            State* out[2] = { nullptr, nullptr };
        };

        //! Specialized class for match states
        struct MatchState : public State
        {
            //! The regular definition name associated with this match state
            std::string defname;
        };
    }
}

#endif // __AXOLOTL_LANG_NFA_STATE_H__
