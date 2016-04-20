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

#ifndef __AXOLOTL_LANG_NFAFRAGMENT_H__
#define __AXOLOTL_LANG_NFAFRAGMENT_H__

#include "lang/forward.hpp"

#include <string>
#include <vector>
#include <map>

namespace lang
{
    namespace nfa
    {
        class Fragment
        {
        public:
            Fragment();
            Fragment(State* start, std::vector<State**> const& dangling);
            Fragment(State* start, std::vector<State**> const& dangling1, std::vector<State**> const& dangling2);

            //! Get this fragment's starting state.
            State* start() const;

            //! Get a list of all dangling (i.e, not connectedd
            //!   outgoing transitions for this fragment.
            std::vector<State**> const& dangling() const;

            //! Patch all the dangling transitions of this fragment
            //!   to point to the given state.
            void patch(State* to);

            //! Build a fragment matching the given character.
            static Fragment match(char c);

            static Fragment string(std::string const& s);

            static Fragment charClass(std::pair<bool, std::vector<std::pair<char, char>>> const& cls);

            //! Build a fragment matching any character.
            static Fragment wildcard();

            //! Concatenate two fragments, that will match sequentially.
            static Fragment concatenate(Fragment e1, Fragment e2);

            //! Create an alternation between two fragments.
            static Fragment alternate(Fragment e1, Fragment e2);

            //! Make a fragment optional.
            static Fragment optional(Fragment e1);

            //! Make a fragment repeating zero or more times.
            static Fragment loop(Fragment e1);

            //! Make a fragment repeating one or more times.
            static Fragment loopone(Fragment e1);

        private:
            State* m_start;
            std::vector<State**> m_dangling;
        };
    }
}

#endif // __AXOLOTL_LANG_NFA_FRAGMENT_H__
