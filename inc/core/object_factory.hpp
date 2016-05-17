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
#include "core/class.hpp"
#include "lang/std_names.hpp"

#include <string>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace core
{
    namespace detail
    {
        static void ObjectFactory_init();
        static void ObjectFactory_fini();

        template <typename T>
        struct Helper
        { static void helper() {} };
    }

    template <typename T>
    static inline std::size_t typeId()
    { return (std::size_t) &detail::Helper<unqualified<T>>::helper; }

    class ObjectFactory
    {
        friend void detail::ObjectFactory_init();
        friend void detail::ObjectFactory_fini();
        
    private:
        typedef std::list<Object> ObjectList;
        typedef std::pair<std::string, Object> NamedObject;
        typedef std::list<NamedObject> NamedObjectList;
        
        struct ConstructorListBuilder
        {
            template <typename T>
            ConstructorListBuilder& operator()(T value)
            {
                list.push_back(value);
                return *this;
            }

            ObjectList list;
        };

        struct MethodListBuilder
        {
            template <typename T>
            MethodListBuilder& operator()(std::string const& name, T value)
            {
                named_list.push_back(NamedObject(name, value));
                return *this;
            }

            NamedObjectList named_list;
        };

    public:
        static ConstructorListBuilder constructorList()
        { return ConstructorListBuilder(); }

        static MethodListBuilder methodList()
        { return MethodListBuilder(); }

        template <typename T>
        static Class const& record(std::string const& name,
                           ConstructorListBuilder const& constructors,
                           MethodListBuilder const& methods)
        {
            Class c(name);

            for (auto const& m : constructors.list)
                c.addMember(name, m);

            for (auto const& m : methods.named_list)
                c.addMember(m.first, m.second);

            m_impl->classes[typeId<T>()] = c;

            return m_impl->classes[typeId<T>()];
        }

        static std::string typeName(std::size_t id)
        {
            if (id == typeId<Object>())
                return lang::std_any_type;

            auto it = m_impl->classes.find(id);
            if (it == m_impl->classes.end())
                throw std::runtime_error("core::ObjectFactory::typeName: object type is not registered");

            return it->second.classname();
        }

        template <typename T>
        static std::string typeName()
        { return typeName(typeId<T>()); }

        static std::size_t typeIdFromName(std::string const& classname)
        {
            auto it = std::find_if(m_impl->classes.begin(), m_impl->classes.end(),
            [=](std::pair<std::size_t, Class> const& item)
            { return item.second.classname() == classname; });

            if (it == m_impl->classes.end())
                throw std::runtime_error("core::ObjectFactory::typeIdFromName: type '" + classname + "' does not exists");
            return it->first;
        }

        static Class const& typeClassFromName(std::string const& name)
        {
            return typeClass(typeIdFromName(name));
        }

        template <typename T>
        static Class const& typeClass()
        {
            return typeClass(typeId<T>());
        }

        static Class const& typeClass(std::size_t id)
        {
            auto it = m_impl->classes.find(id);
            if (it == m_impl->classes.end())
                throw std::runtime_error("core::ObjectFactory::typeClass: type is not registered");

            return it->second;
        }

        static Object unserialize(std::string const& name, std::string const& serialized)
        {
            std::size_t id = typeIdFromName(name);
            return m_impl->classes[id].unserialize(serialized);
        }

        //! Trivial constructor
        static Object construct(Object const& cpy)
        {
            return cpy;
        }

        //! Generic constructor, without finalizer
        template <typename T>
        static Object construct(T value, typename std::enable_if<(not is_callable<T>::value) and (not has_finalizer<T>::value)>::type* dummy = nullptr)
        {
            return constructScalar<T>(std::forward<T>(value));
        }

        //! Generic constructor, with finalizer
        template <typename T>
        static Object construct(T value, typename std::enable_if<(not is_callable<T>::value) and has_finalizer<T>::value>::type* dummy = nullptr)
        {
            Object object = constructScalar<T>(std::forward<T>(value));
            value.finalizeObject(object);
            return object;
        }

        //! Generic scalar constructor
        template <typename T>
        static Object constructScalar(T value)
        {
            if (typeId<T>() == typeId<Object>())
                return value;

            std::map<std::size_t, Class>::iterator it = m_impl->classes.find(typeId<T>());
            if (it == m_impl->classes.end())
                throw std::runtime_error("core::ObjectFactory::construct: object type is not registered");

            return it->second.construct(Some(value));
        }

        //! Direct function pointer
        template <typename TRet, typename... TArgs>
        static Object construct(TRet(*function_ptr)(TArgs...))
        {
            return Object(Object::Kind::Callable, Callable(std::function<TRet(TArgs...)>(function_ptr)), lang::std_callable_classname);
        }

        //! Non-const member functions
        template <typename T, typename TRet, typename... TArgs>
        static Object construct(TRet(T::*member_ptr)(TArgs...))
        {
            auto proxy = [=](T& self, TArgs... args)
            { return (self.*member_ptr)(args...); };
            return construct(std::function<TRet(T&, TArgs...)>(proxy));
        }

        //! Const member functions
        template <typename T, typename TRet, typename... TArgs>
        static Object construct(TRet(T::*member_ptr)(TArgs...) const)
        {
            auto proxy = [=](T const& self, TArgs... args)
            { return (self.*member_ptr)(args...); };
            return construct(std::function<TRet(T const&, TArgs...)>(proxy));
        }

        //! Functors and lambdas
        template <typename T>
        static Object construct(T const& callable, typename std::enable_if<is_callable<T>::value>::type* dummy = nullptr)
        {
            return Object(Object::Kind::Callable, Callable(std::function<get_signature<T>>(callable)), lang::std_callable_classname);
        }

        //! Direct std::function use
        template <typename TRet, typename... TArgs>
        static Object construct(std::function<TRet(TArgs...)> const& fun)
        {
            return Object(Object::Kind::Callable, Callable(fun), lang::std_callable_classname);
        }

    private:
        static struct Impl
        {
            std::map<std::size_t, Class> classes;
        }* m_impl;
    };
}

#endif // __AXOLOTL_CORE_OBJECTFACTORY_H__
