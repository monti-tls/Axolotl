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
#include <list>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <stdexcept>

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
        typedef std::function<Object(Some const&)> Interface;
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
        static void record(std::string const& name,
                                 ConstructorListBuilder const& constructors,
                                 MethodListBuilder const& methods)
        {
            std::size_t type = typeId<T>();
            m_impl->names[type] = name;
            m_impl->constructors[type] = constructors.list;
            m_impl->methods[type] = methods.named_list;
            m_impl->interfaces[type] = [=](Some const& value) -> Object
            {
                Object object(Object::Kind::Scalar, value, name);
                Object::setupBuiltinMembers(object);
                for (auto method : methods.named_list)
                    object.newPolymorphic(method.first) = method.second;
                return object;
            };
        }

        template <typename T>
        static void record(std::string const& name, std::function<Object(T)> const& iface)
        {
            std::size_t type = typeId<T>();
            m_impl->names[type] = name;
            m_impl->interfaces[type] = [=](Some const& value) -> Object { return iface(const_cast<T&>(value.as<unqualified<T>>())); };
        }

        template <typename T>
        static std::string typeName()
        {
            auto it = m_impl->names.find(typeId<T>());
            if (it == m_impl->names.end())
                throw std::runtime_error("core::ObjectFactory::typeName: object type is not registered");

            return it->second;
        }

        static std::size_t typeIdFromName(std::string const& classname)
        {
            auto it = std::find_if(m_impl->names.begin(), m_impl->names.end(),
                [=](std::pair<std::size_t, std::string> const& item) { return item.second == classname; });
            if (it == m_impl->names.end())
                throw std::runtime_error("core::ObjectFactory::typeIdFromName: type '" + classname + "' does not exists");
            return it->first;
        }

        static ObjectList const& typeConstructors(std::string const& classname)
        { return m_impl->constructors[typeIdFromName(classname)]; }

        static Object const& typeMethod(std::string const& classname, std::string const& name)
        {
            // Get typeId
            std::size_t typeId = typeIdFromName(classname);

            // Get method list
            NamedObjectList const& methods = m_impl->methods[typeId];

            // Check if method exists and if its not polymorphic
            auto mpred = [=](NamedObject const& item) { return item.first == name; };
            std::size_t count = std::count_if(methods.begin(), methods.end(), mpred);

            if (!count)
                throw std::runtime_error("core::ObjectFactory::typeMethod: method '" + name + "' in class '" + classname + "' does not exists");
            else if (count > 1)
                throw std::runtime_error("core::ObjectFactory::typeMethod: method '" + name + "' in class '" + classname + "' is polymorphic");

            // Fetch the method (this is guaranteed to success because of the checks above)
            return std::find_if(methods.begin(), methods.end(), mpred)->second;
        }

        static Object build(Object const& cpy)
        { return cpy; }

        //! Generic constructor
        template <typename T>
        static Object build(T value, typename std::enable_if<not is_callable<T>::value>::type* dummy = nullptr)
        {
            std::map<std::size_t, Interface>::iterator it = m_impl->interfaces.find(typeId<T>());
            if (it == m_impl->interfaces.end())
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
        static struct Impl
        {
            std::map<std::size_t, Interface> interfaces;
            std::map<std::size_t, std::string> names;
            std::map<std::size_t, ObjectList> constructors;
            std::map<std::size_t, NamedObjectList> methods;
        }* m_impl;
    };
}

#endif // __AXOLOTL_CORE_OBJECTFACTORY_H__
