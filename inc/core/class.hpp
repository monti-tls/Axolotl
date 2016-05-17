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

#ifndef __AXOLOTL_CORE_CLASS_H__
#define __AXOLOTL_CORE_CLASS_H__

#include "core/forward.hpp"
#include "core/some.hpp"

#include <string>
#include <list>

namespace core
{
    class Class
    {
    public:
        typedef std::pair<std::string, Object> Member;

    public:
        Class(std::string const& classname = "");
        ~Class();

        std::string const& classname() const;
        void addMember(std::string const& name, Object const& value);

        Object construct(Some const& value = Some()) const;
        Object unserialize(std::string const& serialized) const;

        void finalizeObject(Object& self) const;

    private:
        std::string m_classname;
        std::list<Member> m_members;
    };
}

#endif // __AXOLOTL_CORE_CLASS_H__
