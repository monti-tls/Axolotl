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

Class::Id Class::AnyId;
Class Class::AnyClass = Class();

Class::Class()
    : m_impl(nullptr)
{}

Class::Class(std::string const& module_name, std::string const& classname, bool in_script)
    : m_impl(new Impl())
{
    m_impl->refcount = 1;
    m_impl->classid = hashId(classname, module_name);
    m_impl->classname = classname;
    m_impl->in_script = in_script;

    addMember(lang::std_classname, classname);
    addMember(lang::std_classid, m_impl->classid);

    addMember(lang::std_equals,
    [](Object const& self, Object const& obj)
    {
        if (self.classname() != obj.classname())
            return false;
        return self.serialize() == obj.serialize();
    });

    addMember(lang::std_lt,
    [](Object const& self, Object const& obj)
    { return (self.classname() + self.serialize()) < (obj.classname() + obj.serialize()); });

    addMember(lang::std_lte,
    [](Object const& self, Object const& obj)
    { return (self < obj) || (self == obj); });

    addMember(lang::std_gt,
    [](Object const& self, Object const& obj)
    { return obj < self; });

    addMember(lang::std_gte,
    [](Object const& self, Object const& obj)
    { return (self > obj) || (self == obj); });

    addMember(lang::std_nequals,
    [](Object const& self, Object const& obj)
    { return self != obj; });
}

Class::Class(Class const& cpy)
    : m_impl(cpy.m_impl)
{ M_incref(); }

Class::~Class()
{ M_decref(); }

Class& Class::operator=(Class const& cpy)
{
    M_decref();
    m_impl = cpy.m_impl;
    M_incref();
    return *this;
}

Class::Id Class::classid() const
{ return m_impl->classid; }

std::string const& Class::classname() const
{ return m_impl->classname; }

void Class::addMember(std::string const& name, Object const& value)
{ m_impl->members.push_back(std::make_pair(name, value)); }

Object Class::construct(Some&& value) const
{
    Object object(std::forward<Some>(value), *this);

    for (auto m : m_impl->members)
        object.newPolymorphic(m.first) = m.second;

    return object;
}

Object Class::unserialize(std::string const& value) const
{
    for (auto m : m_impl->members)
        if (m.first == lang::std_unserialize)
            return m.second.invoke({ value });

    return Object::nil();
}

void Class::finalizeObject(Object& self) const
{
    if (m_impl->in_script)
    {
        Class thisclass = *this;
        auto proxy = [=](std::vector<Object> o)
        {
            o.erase(o.begin());
            
            Object self = thisclass.construct();
            o.insert(o.begin(), self);
            self.invokePolymorphic(thisclass.classname(), o);
            return self;
        };
        self.newPolymorphic(lang::std_call) = Callable(std::function<Object(std::vector<Object>)>(proxy), true);
    }
    else
    {
        Class thisclass = *this;
        auto proxy = [=](std::vector<Object> o)
        {
            o.erase(o.begin());

            for (auto const& m : thisclass.m_impl->members)
            {
                if (m.first != thisclass.classname())
                    continue;

                if (m.second.callable() && m.second.unwrap<Callable>().signature().match(o))
                    return m.second.invoke(o);
            }

            return Object::nil();
        };
        self.newPolymorphic(lang::std_call) = Callable(std::function<Object(std::vector<Object>)>(proxy), true);
    }

    // No constructors were provided
    /*if (!self.has(lang::std_call))
    {
        self.newPolymorphic(lang::std_call) =
        [](Object const&) { throw std::runtime_error("object is not constructible"); };
    }*/
}

Object& Class::operator[](std::string const& name)
{
    m_impl->members.push_back(std::make_pair(name, Object()));
    return m_impl->members.back().second;
}

Object& Class::operator[](const char* name)
{
    return operator[](std::string(name));
}

Class::Id Class::hashId(std::string const& classname, std::string const& module_name)
{ return std::hash<std::string>{}(classname + "." + module_name); }

void Class::M_incref()
{
    if (m_impl)
        ++m_impl->refcount;
}

void Class::M_decref()
{
    if (m_impl && !--m_impl->refcount)
    {
        delete m_impl;
        m_impl = nullptr;
    }
}
