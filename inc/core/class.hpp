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
#include <map>
#include <memory>

namespace core
{
    class Class
    {
    public:
        typedef std::size_t Id;
        typedef std::pair<std::string, Object> Member;

        static Id AnyId;
        static Class AnyClass;
    public:
        Class();
        Class(std::string const& module_name, std::string const& classname, bool in_script = false);
        Class(Class const& cpy);
        ~Class();

        Class& operator=(Class const& cpy);

        Id classid() const;
        std::string const& classname() const;
        void addMember(std::string const& name, Object value);

        Object construct(Some&& value = Some()) const;
        Object unserialize(std::string const& serialized) const;

        void finalizeObject(Object& self) const;
        Object& operator[](std::string const& name);
        Object& operator[](const char* name);

    private:
        void M_incref();
        void M_decref();

    private:
        struct Impl
        {
            Id classid;
            std::string classname;
            std::list<Member> members;
            bool in_script;
            int refcount;
        }* m_impl;

    public:
        static Id hashId(std::string const& module_name, std::string const& classname);
        static std::string hashIdClassname(Id classid);

    private:
        static std::map<Id, std::string> m_classnames;
    };
}

#endif // __AXOLOTL_CORE_CLASS_H__
