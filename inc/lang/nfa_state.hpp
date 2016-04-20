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
        struct CharClass
        {
            int low, high;
            CharClass* next = nullptr;
        };

        struct State
        {
            enum Kind
            {
                Wildcard,
                Range,
                Split,
                Match
            };

            int what;
            CharClass* char_class = nullptr;
            bool invert = false;

            State* out[2] = { nullptr, nullptr };
        };

        struct MatchState : public State
        {
            std::string defname;
        };
    }
}

#endif // __AXOLOTL_LANG_NFA_STATE_H__
