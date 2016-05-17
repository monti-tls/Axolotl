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

#ifndef __AXOLOTL_CORE_PPACK_H
#define __AXOLOTL_CORE_PPACK_H

#include "core/object.hpp"

#include <type_traits>
#include <utility>
#include <tuple>
#include <vector>
#include <functional>

namespace core
{
    template <size_t N>
    struct ApplyTuple
    {
        template <typename F, typename T, typename... A>
        static inline auto applyTuple(F&& f, T&& t, A&&... a) -> decltype(ApplyTuple<N - 1>::applyTuple(::std::forward<F>(f), ::std::forward<T>(t), ::std::get<N - 1>(::std::forward<T>(t)), ::std::forward<A>(a)...))
        {
            return ApplyTuple<N - 1>::applyTuple(::std::forward<F>(f), ::std::forward<T>(t), ::std::get<N - 1>(::std::forward<T>(t)), ::std::forward<A>(a)...);
        }
    };

    template <>
    struct ApplyTuple<0>
    {
        template <typename F, typename T, typename... A>
        static inline auto applyTuple(F&& f, T&&, A&&... a) -> decltype(::std::forward<F>(f)(::std::forward<A>(a)...))
        {
            return ::std::forward<F>(f)(::std::forward<A>(a)...);
        }
    };

    template <typename F, typename T>
    inline auto applyTuple(F&& f, T&& t) -> decltype(ApplyTuple<::std::tuple_size<typename ::std::decay<T>::type>::value>::applyTuple(::std::forward<F>(f), ::std::forward<T>(t)))
    {
        return ApplyTuple<::std::tuple_size<typename ::std::decay<T>::type>::value>::applyTuple(::std::forward<F>(f), ::std::forward<T>(t));
    }

    static std::tuple<> vec2tuple(std::vector<Object>::iterator)
    { return std::tuple<>(); }
    
    inline Object& unwrap_object(Object& object, Object const*)
    { return object; }

    template <typename T>
    inline T& unwrap_object(Object& object, T*,
                            typename std::enable_if<
                                not std::is_same<
                                    typename std::remove_const<T>::type,
                                    Object
                                >::value
                            >::type* dummy = nullptr)
    { return object.unwrap<T>(); }

    template <typename A, typename... Args>
    static std::tuple<A&, Args&...> vec2tuple(std::vector<Object>::iterator it, A*, Args*... args)
    {
        std::tuple<A&> first = std::tie(unwrap_object(*it, (A*) nullptr));
        std::tuple<Args&...> second = vec2tuple(++it, args...);
        return std::tuple_cat(first, second);
    }

    static std::vector<Object> pack2vec()
    { return std::vector<Object>(); }

    template <typename A, typename... Args>
    static std::vector<Object> pack2vec(A const& a, Args const&... args)
    {
        std::vector<Object> vec = pack2vec(args...);
        vec.insert(vec.begin(), a);
        return vec;
    }

    template <typename T>
    struct always_string
    { typedef std::string type; };

    template <typename T>
    static std::vector<T> explicit_pack2vec()
    { return std::vector<T>(); }

    template <typename T, typename A>
    static std::vector<T> explicit_pack2vec(A const& a)
    { return std::vector<T>(1, a); }

    template <typename T, typename A, typename... Args>
    static std::vector<T> explicit_pack2vec(A const& a, Args const&... args)
    {
        std::vector<T> vec = explicit_pack2vec<T, Args...>(args...);
        vec.insert(vec.begin(), a);
        return vec;
    }

    template <typename T>
    class is_callable_impl
    {
    private:
        typedef char(&yes)[1];
        typedef char(&no)[2];

        struct fallback { void operator()(); };
        struct derived : T, fallback {};

        template <typename U, U> struct check;

        template <typename>
        static yes test(...);

        template <typename C>
        static no test(check<void (fallback::*)(), &C::operator()>*);

    public:
        static constexpr bool value = sizeof(test<derived>(0)) == sizeof(yes);
    };

    template<typename T>
    struct is_callable
    : std::conditional<
        std::is_function<T>::value,
        std::true_type,
        typename std::conditional<
            std::is_class<T>::value,
            is_callable_impl<T>,
            std::false_type
        >::type
    >::type {};

    template <typename T>
    class has_finalizer_impl
    {
    private:
        typedef char(&yes)[1];
        typedef char(&no)[2];

        struct fallback { void finalizeObject(); };
        struct derived : T, fallback {};

        template <typename U, U> struct check;

        template <typename>
        static yes test(...);

        template <typename C>
        static no test(check<void (fallback::*)(), &C::finalizeObject>*);

    public:
        static constexpr bool value = sizeof(test<derived>(0)) == sizeof(yes);
    };

    template<typename T>
    struct has_finalizer
    : std::conditional<
        std::is_class<T>::value,
        has_finalizer_impl<T>,
        std::false_type
    >::type {};

    template<typename T> struct remove_class { using type = void; };
    template<typename C, typename R, typename... A>
    struct remove_class<R(C::*)(A...)> { using type = R(A...); };
    template<typename C, typename R, typename... A>
    struct remove_class<R(C::*)(A...) const> { using type = R(A...); };
    template<typename C, typename R, typename... A>
    struct remove_class<R(C::*)(A...) volatile> { using type = R(A...); };
    template<typename C, typename R, typename... A>
    struct remove_class<R(C::*)(A...) const volatile> { using type = R(A...); };

    template<typename T>
    struct get_signature_impl { using type = typename remove_class<
        decltype(&std::remove_reference<T>::type::operator())>::type; };
    template<typename R, typename... A>
    struct get_signature_impl<R(A...)> { using type = R(A...); };
    template<typename R, typename... A>
    struct get_signature_impl<R(&)(A...)> { using type = R(A...); };
    template<typename R, typename... A>
    struct get_signature_impl<R(*)(A...)> { using type = R(A...); };
    template<typename T> using get_signature = typename get_signature_impl<T>::type;

    template <typename T>
    static std::function<get_signature<T>> lambda2function(T const& lambda)
    { return std::function<get_signature<T>>(lambda); }

    template <typename T>
    using unqualified = typename std::remove_const<typename std::remove_reference<T>::type>::type;
}

#endif // __AXOLOTL_CORE_PPACK_H
