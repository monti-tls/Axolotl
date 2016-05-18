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

#include "core/class.hpp"
#include "core/core.hpp"
#include "lang/std_names.hpp"

#include <functional>

using namespace core;

constexpr Class::ClassId Class::AnyClassId;

Class::Class(std::string const& classname, std::string const& module_name)
    : m_classid(hashClassId(classname, module_name))
    , m_classname(classname)
{}

Class::~Class()
{}

Class::ClassId Class::classid() const
{ return m_classid; }

std::string const& Class::classname() const
{ return m_classname; }

void Class::addMember(std::string const& name, Object const& value)
{ m_members.push_back(std::make_pair(name, value)); }

Object Class::construct(Some&& value) const
{
    Object object(Object::Kind::Scalar, std::forward<Some>(value), m_classname, m_classid);
    Object::setupBuiltinMembers(object);

    for (auto m : m_members)
        object.newPolymorphic(m.first) = m.second.copy();

    return object;
}

Object Class::unserialize(std::string const& value) const
{
    for (auto m : m_members)
        if (m.first == lang::std_unserialize)
            return m.second.invoke({ value });

    return Object::nil();
}

void Class::finalizeObject(Object& self) const
{
    for (auto m : m_members)
        self.newPolymorphic(m.first) = m.second;

    for (auto m : m_members)
    {
        if (m.first == m_classname)
        {
            Class thisclass = *this;
            auto proxy = [=](Object const& o)
            {
                Object self = thisclass.construct();
                m.second(self, o);
                return self;
            };

            self.newPolymorphic(lang::std_call) = proxy;
        }
    }

    // No constructors were provided
    if (!self.has(lang::std_call))
    {
        self.newPolymorphic(lang::std_call) =
        [](Object const&) { throw std::runtime_error("object is not constructible"); };
    }
}

Class::ClassId Class::hashClassId(std::string const& classname, std::string const& module_name)
{ return std::hash<std::string>{}(classname + "." + module_name); }
