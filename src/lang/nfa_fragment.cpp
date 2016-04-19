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

#include "lang/nfa_fragment.hpp"
#include "lang/nfa_state.hpp"

#include <stdexcept>

using namespace lang;
using namespace nfa;

Fragment::Fragment()
    : m_start(nullptr)
{}

Fragment::Fragment(State* start, std::vector<State**> const& dangling)
    : m_start(start)
    , m_dangling(dangling)
{}

Fragment::Fragment(State* start, std::vector<State**> const& dangling1, std::vector<State**> const& dangling2)
    : m_start(start)
    , m_dangling(dangling1)
{
    m_dangling.insert(m_dangling.end(),
                      std::make_move_iterator(dangling2.begin()),
                      std::make_move_iterator(dangling2.end()));
}


State* Fragment::start() const
{ return m_start; }

std::vector<State**> const& Fragment::dangling() const
{ return m_dangling; }

void Fragment::patch(State* to)
{
    for (State** out : m_dangling)
    {
        if (!out)
            throw std::runtime_error("Fragment::patch: null pointer");
        *out = to;
    }

    m_dangling.clear();
}

Fragment Fragment::match(char c)
{
    State* state = new State;
    state->what = (int) c;
    return Fragment(state, { &state->out[0] });
}

Fragment Fragment::string(std::string const& s)
{
    if (!s.size())
        throw std::runtime_error("Fragment::string: string is empty");

    Fragment frag = match(s[0]);

    for (int i = 1; i < (int) s.size(); ++i)
        frag = concatenate(frag, match(s[i]));

    return frag;
}

Fragment Fragment::wildcard()
{
    State* state = new State;
    state->what = State::Wildcard;
    return Fragment(state, { &state->out[0] });
}

Fragment Fragment::concatenate(Fragment e1, Fragment e2)
{
    //!  +----+   +----+
    //! >| e1 |-->| e2 |-->
    //!  +----+   +----+

    e1.patch(e2.start());
    return Fragment(e1.start(), e2.dangling());
}

Fragment Fragment::alternate(Fragment e1, Fragment e2)
{
    //!      +----+
    //!  +-->| e1 |-->
    //!  |   +----+
    //! >O
    //!  |   +----+
    //!  +-->| e2 |-->
    //!      +----+

    State* state = new State;
    state->what = State::Split;
    state->out[0] = e1.start();
    state->out[1] = e2.start();
    return Fragment(state, e1.dangling(), e2.dangling());
}

Fragment Fragment::optional(Fragment e1)
{
    //!      +----+
    //!  +-->| e1 |-->
    //!  |   +----+
    //! >O----------->

    State* state = new State;
    state->what = State::Split;
    state->out[0] = e1.start();
    return Fragment(state, e1.dangling(), { &state->out[1] });
}

Fragment Fragment::loop(Fragment e1)
{
    //!      +----+
    //!  +-->| e1 |--+
    //!  |   +----+  |
    //! >O<----------+
    //!  |
    //!  +------------>

    State* state = new State;
    state->what = State::Split;
    state->out[0] = e1.start();
    e1.patch(state);
    return Fragment(state, { &state->out[1] });
}

Fragment Fragment::loopone(Fragment e1)
{
    //!     +-----+
    //!     v     |
    //!  +----+   | 
    //! >| e1 |-->O-->
    //!  +----+

    State* state = new State;
    state->what = State::Split;
    state->out[0] = e1.start();
    e1.patch(state);
    return Fragment(e1.start(), { &state->out[1] });
}
