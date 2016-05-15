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
            //! Create an empty fragment
            Fragment();

            //! Create a fragment with start state `start' and with outgoing
            //!   dangling transitions `dangling'
            //! \param start The start state of the fragment
            //! \param dangling The list of the dangling outgoing transitions of the fragment
            Fragment(State* start, std::vector<State**> const& dangling);

            //! Create a fragment with start state `start', forming the list of outgoing
            //!   dangling transitions by merging the two vectors `dangling1' and `dangling2'
            //! \param start The start state of the fragment
            //! \param dangling1 The first list of outgoing dangling transitions
            //! \param dangling1 The second list of outgoing dangling transitions
            Fragment(State* start, std::vector<State**> const& dangling1, std::vector<State**> const& dangling2);

            //! Get this fragment's starting state.
            //! \return This fragment's starting state
            State* start() const;

            //! Get a list of all dangling (i.e, not connectedd
            //!   outgoing transitions for this fragment.
            //! \return The list of dangling outgoing transitions of this fragment
            std::vector<State**> const& dangling() const;

            //! Patch all the dangling transitions of this fragment
            //!   to point to the given state.
            //! \param to The state to which all dangling transitions should
            //!           be patched
            void patch(State* to);

            //! Build a fragment matching the given character.
            //! \param c The character to match
            //! \return The created fragment
            static Fragment match(char c);

            //! Build a fragment matching the given string
            //! \param s The string to match
            //! \return The created fragment
            static Fragment string(std::string const& s);

            //! Build a fragment matching a character class
            //! \param cls.first true if the character class is negated
            //! \param cls.second A vector of pairs representing character ranges
            //! \return The created fragment
            static Fragment charClass(std::pair<bool, std::vector<std::pair<char, char>>> const& cls);

            //! Build a fragment matching any character.
            //! \return The created fragment
            static Fragment wildcard();

            //! Concatenate two fragments, that will match sequentially.
            //! \param e1 The first fragment
            //! \param e2 The second fragment
            //! \return The created fragment
            static Fragment concatenate(Fragment e1, Fragment e2);

            //! Create an alternation between two fragments.
            //! \param e1 The first fragment
            //! \param e2 The second fragment
            //! \return The created fragment
            static Fragment alternate(Fragment e1, Fragment e2);

            //! Make a fragment optional.
            //! \param e1 The fragment to turn optional
            //! \return The created fragment
            static Fragment optional(Fragment e1);

            //! Make a fragment repeating zero or more times.
            //! \param e1 The fragment to repeat zero or more times
            //! \return The created fragment
            static Fragment loop(Fragment e1);

            //! Make a fragment repeating one or more times.
            //! \param e1 The fragment to repeat one or more times
            //! \return The created fragment
            static Fragment loopone(Fragment e1);

        private:
            State* m_start;
            std::vector<State**> m_dangling;
        };
    }
}

#endif // __AXOLOTL_LANG_NFA_FRAGMENT_H__
