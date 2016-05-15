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

#ifndef __AXOLOTL_CORE_OBJECT_H__
#define __AXOLOTL_CORE_OBJECT_H__

#include "core/some.hpp"

#include <vector>
#include <string>
#include <map>

namespace core
{
    class Object
    {
    public:
        enum class Kind
        {
            Nil,
            Callable,
            Scalar
        };

    public:
        Object();
        Object(Object const& cpy);
        Object(Object const& cpy, bool weaken);
        template <typename T>
        Object(T value);
        Object(Kind kind, Some meta, std::string const& classname);
        ~Object();

        Object& operator=(Object const& cpy);

        bool isWeak() const;
        Object weakref() const;

        Kind kind() const;
        Some const& meta() const;

        bool isScalar() const;
        bool isCallable() const;
        bool isInvokable() const;
        bool isNil() const;
        std::string classname() const;

        bool has(std::string const& id) const;
        bool isPolymorphic(std::string const& id) const;
        Object& newPolymorphic(std::string const& id);
        Object const& findPolymorphic(std::string const& id, std::vector<Object> const& args) const;

        Object& member(std::string const& id);
        Object const& member(std::string const& id) const;
        Object invokeMember(std::string const& name, std::vector<Object> const& args) const;
        Object invokePolymorphic(std::string const& name, std::vector<Object> const& args) const;
        Object invoke(std::vector<Object> const& args) const;
        Object method(std::string const& name, std::vector<Object> const& args) const;

        template <typename T>
        T& unwrap();
        template <typename T>
        T const& unwrap() const;

        template <typename... Args>
        Object operator()(Args const&... args) const;

        Object operator==(Object const& other) const;
        Object operator!=(Object const& other) const;
        Object operator&&(Object const& other) const;
        Object operator||(Object const& other) const;
        Object operator!() const;
        Object operator+(Object const& other) const;
        Object operator-(Object const& other) const;
        Object operator*(Object const& other) const;
        Object operator/(Object const& other) const;
        Object operator%(Object const& other) const;
        Object operator<(Object const& other) const;
        Object operator<=(Object const& other) const;
        Object operator>(Object const& other) const;
        Object operator>=(Object const& other) const;

        operator bool() const;

        static Object const& nil();
        static void setupBuiltinMembers(Object& obj);

    private:
        void M_incref();
        void M_decref();
        void M_destroy();

    private:
        bool m_weak;
        struct Impl
        {
            Kind kind;
            Some meta;
            std::string classname;
            std::multimap<std::string, Object> members;
            int refcount;
        }* m_impl;

    private:
        static Object m_nil;
        static bool m_nil_inited;
    };
}

#endif // __AXOLOTL_CORE_OBJECT_H__
