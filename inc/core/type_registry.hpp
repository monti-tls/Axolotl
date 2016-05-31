#ifndef __AXOLOTL_CORE_TYPE_REGISTRY_H__
#define __AXOLOTL_CORE_TYPE_REGISTRY_H__

#include "core/ppack.hpp"
#include "core/some.hpp"
#include "core/class.hpp"

#include <map>
#include <string>
#include <stdexcept>

namespace core
{
    namespace detail
    {
        static void type_registry_init();
        static void type_registry_fini();

        template <typename T>
        struct UniqueTypeIdHelper
        { static void helper() {} };

        template <typename T>
        static inline std::size_t uniqueTypeId()
        { return reinterpret_cast<std::size_t>(&UniqueTypeIdHelper<unqualified<T>>::helper); }

        extern std::map<std::size_t, Class>* type_registry;
    }

    static inline Class const& associate(std::size_t typeId, Class const& c)
    {
        auto it = detail::type_registry->find(typeId);
        if (it != detail::type_registry->end())
            throw std::runtime_error("core::associate: type is already associated with class `" + it->second.classname() + "'");

        detail::type_registry->insert(std::make_pair(typeId, c));
        return c;
    }

    template <typename T>
    static inline Class const& associate(Class const& c)
    { return associate(detail::uniqueTypeId<T>(), c); }

    static inline Class const& type_class(std::size_t typeId)
    {
        auto it = detail::type_registry->find(typeId);
        if (it == detail::type_registry->end())
            throw std::runtime_error("core::typeClass: type is not associated");

        return it->second;
    }

    template <typename T>
    static inline Class const& type_class()
    { return type_class(detail::uniqueTypeId<T>()); }

    static inline std::size_t class_type(Class::Id classid)
    {
        for (auto it : *detail::type_registry)
        {
            if (it.second.classid() == classid)
                return it.first;
        }

        throw std::runtime_error("core::classType: class is not associated with a type");
    }

    static inline std::size_t class_type(Class const& c)
    { return class_type(c.classid()); }

    static inline Class const& class_from_classid(Class::Id const& classid)
    {
        for (auto const& it : *detail::type_registry)
        {
            if (it.second.classid() == classid)
                return it.second;
        }

        throw std::runtime_error("core::class_from_classid: ??");
    }

    template <typename T>
    static inline Object construct_scalar(T const& value)
    {
        std::size_t typeId = detail::uniqueTypeId<T>();

        if (typeId == detail::uniqueTypeId<Object>())
            return value;

        if (detail::type_registry)
        {
            auto it = detail::type_registry->find(typeId);

            if (it != detail::type_registry->end())
            {
                return it->second.construct(Some(value));
            }
        }

        // Construct a pending object since the class has not been
        //   associated with the type (yet, hopefully)
        return Object(value, typeId);
    }

    static inline Object construct(Object const& object)
    { return object; }

    template <typename T>
    static inline Object construct(T const& value, typename std::enable_if<(not is_callable<T>::value) and (not has_finalizer<T>::value)>::type* dummy = nullptr)
    {
        return construct_scalar(value);
    }

    template <typename T>
    static inline Object construct(T const& value, typename std::enable_if<(not is_callable<T>::value) and has_finalizer<T>::value>::type* dummy = nullptr)
    {
        Object object = construct_scalar(value);
        if (!object.pending())
            value.finalizeObject(object);
        return object;
    }

    template <typename TRet, typename... TArgs>
    static inline Object construct(TRet(*function_ptr)(TArgs...))
    {
        return construct_scalar(Callable(std::function<TRet(TArgs...)>(function_ptr)));
    }

    /*template <typename T, typename... TArgs>
    static inline Object construct(void(T::*member_ptr)(TArgs...))
    {
        auto proxy = [=](T& self, TArgs... args)
        { (self.*member_ptr)(args...); };
        return construct_scalar(std::function<void(T&, TArgs...)>(proxy));
    }*/

    template <typename T, typename TRet, typename... TArgs>
    static inline Object construct(TRet(T::*member_ptr)(TArgs...))
    {
        // Force copy on return for reference types
        using U = unqualified<TRet>;
        auto proxy = [=](T& self, TArgs... args) -> U
        { return (self.*member_ptr)(args...); };
        return construct(std::function<U(T&, TArgs...)>(proxy));
    }

    template <typename T, typename TRet, typename... TArgs>
    static inline Object construct(TRet(T::*member_ptr)(TArgs...) const)
    {
        // Force copy on return for reference types
        using U = unqualified<TRet>;
        auto proxy = [=](T const& self, TArgs... args) -> U
        { return (self.*member_ptr)(args...); };
        return construct(std::function<U(T const&, TArgs...)>(proxy));
    }

    template <typename T>
    static inline Object construct(T const& callable, typename std::enable_if<is_callable<T>::value>::type* dummy = nullptr)
    {
        return construct_scalar(Callable(std::function<get_signature<T>>(callable)));
    }

    template <typename TRet, typename... TArgs>
    static inline Object construct(std::function<TRet(TArgs...)> const& fun)
    {
        return construct_scalar(Callable(fun));
    }
}

#endif // __AXOLOTL_CORE_TYPE_REGISTRY_H__
