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

#ifndef __AXOLOTL_CORE_OBJECTFACTORY_H__
#define __AXOLOTL_CORE_OBJECTFACTORY_H__

#include "core/some.hpp"
#include "core/object.hpp"
#include "core/callable.hpp"
#include "lang/std_names.hpp"

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <stdexcept>

namespace core
{
    namespace detail
    {
        template <typename T>
        struct Helper
        { static void helper() {} };
    }

    template <typename T>
    static inline std::size_t typeId()
    { return (std::size_t) &detail::Helper<unqualified<T>>::helper; }

    class ObjectFactory
    {
    private:
        typedef std::function<Object(Some const&)> Iface;

        struct MemberList
        {
            template <typename T>
            MemberList& operator()(std::string const& name, T value)
            {
                as_vector.push_back(std::pair<std::string, Object>(name, value));
                return *this;
            }

            std::vector<std::pair<std::string, Object>> as_vector;
        };

    public:
        static MemberList memberList()
        { return MemberList(); }

        template <typename T>
        static void registerType(std::string const& name, MemberList const& members)
        {
            std::size_t type = typeId<T>();
            m_names[type] = name;
            m_members[type] = members.as_vector;
            m_ifaces[type] = [=](Some const& value) -> Object
            {
                Object object(Object::Kind::Scalar, value, name);
                Object::setupBuiltinMembers(object);
                for (auto member : members.as_vector)
                    object.newPolymorphic(member.first) = member.second;
                return object;
            };
        }

        template <typename T>
        static void registerType(std::string const& name, std::function<Object(T)> const& iface)
        {
            std::size_t type = typeId<T>();
            m_names[type] = name;
            m_ifaces[type] = [=](Some const& value) -> Object { return iface(const_cast<T&>(value.as<unqualified<T>>())); };
        }

        template <typename T>
        static std::string typeName()
        {
            auto it = m_names.find(typeId<T>());
            if (it == m_names.end())
                throw std::runtime_error("core::ObjectFactory::typeName: object type is not registered");

            return it->second;
        }

        static Object const& typeMember(std::string const& classname, std::string const& name)
        {
            // Get typeId
            auto it = std::find_if(m_names.begin(), m_names.end(),
                [=](std::pair<std::size_t, std::string> const& item) { return item.second == classname; });
            if (it == m_names.end())
                throw std::runtime_error("core::ObjectFactory::typeMember: type '" + classname + "' does not exists");
            std::size_t typeId = it->first;

            // Get member list
            std::vector<std::pair<std::string, Object>> const& members = m_members[typeId];

            // Check if member exists and if its not polymorphic
            auto mpred = [=](std::pair<std::string, Object> const& item) { return item.first == name; };
            std::size_t count = std::count_if(members.begin(), members.end(), mpred);

            if (!count)
                throw std::runtime_error("core::ObjectFactory::typeMember: member '" + name + "' in class '" + classname + "' does not exists");
            else if (count > 1)
                throw std::runtime_error("core::ObjectFactory::typeMember: member '" + name + "' in class '" + classname + "' is polymorphic");

            // Fetch the member
            auto mit = std::find_if(members.begin(), members.end(), mpred);
            return mit->second;
        }

        static Object build(Object const& cpy)
        { return cpy; }

        //! Generic constructor
        template <typename T>
        static Object build(T value, typename std::enable_if<not is_callable<T>::value>::type* dummy = nullptr)
        {
            std::map<std::size_t, Iface>::iterator it = m_ifaces.find(typeId<T>());
            if (it == m_ifaces.end())
                throw std::runtime_error("core::ObjectFactory::build: object type is not registered");

            return it->second(Some(value));
        }

        //! Direct function pointer
        template <typename TRet, typename... TArgs>
        static Object build(TRet(*function_ptr)(TArgs...))
        { return Object(Object::Kind::Callable, Callable(std::function<TRet(TArgs...)>(function_ptr)), lang::std_callable_classname); }

        //! Functors and lambdas
        template <typename T>
        static Object build(T const& callable, typename std::enable_if<is_callable<T>::value>::type* dummy = nullptr)
        { return Object(Object::Kind::Callable, Callable(std::function<get_signature<T>>(callable)), lang::std_callable_classname); }

        //! Direct std::function use
        template <typename TRet, typename... TArgs>
        static Object build(std::function<TRet(TArgs...)> const& fun)
        { return Object(Object::Kind::Callable, Callable(fun), lang::std_callable_classname); }

    private:
        static std::map<std::size_t, Iface> m_ifaces;
        static std::map<std::size_t, std::string> m_names;
        static std::map<std::size_t, std::vector<std::pair<std::string, Object>>> m_members;
    };
}

#endif // __AXOLOTL_CORE_OBJECTFACTORY_H__
