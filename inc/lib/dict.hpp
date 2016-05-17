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

#ifndef __AXOLOTL_LIB_DICT_H__
#define __AXOLOTL_LIB_DICT_H__

#include "core/core.hpp"
#include "vm/forward.hpp"

#include <map>

namespace lib
{
    class Dict
    {
    public:
        typedef std::map<core::Object, core::Object>::iterator iterator;
        typedef std::map<core::Object, core::Object>::const_iterator const_iterator;

    public:
        static void record();
        static vm::Module const& module();

    public:
        Dict();
        ~Dict();

        iterator begin();
        const_iterator begin() const;

        iterator end();
        const_iterator end() const;

        iterator find(core::Object const& key);
        const_iterator find(core::Object const& key) const;

        void set(core::Object const& key, core::Object const& value);

    private:
        std::map<core::Object, core::Object> m_impl;
    };
}

#endif // __AXOLOTL_LIB_DICT_H__
