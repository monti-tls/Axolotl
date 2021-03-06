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

#ifndef __AXOLOTL_CORE_CALLBACK_IMPL_H__
#define __AXOLOTL_CORE_CALLBACK_IMPL_H__

#include "core/ppack.hpp"
#include "core/exception.hpp"

#include <stdexcept>

namespace core
{
    template <typename TRet, typename... TArgs>
    Object CallbackImpl<TRet, TArgs...>::invoke(std::vector<Object> args)
    {   
        if (this->m_variadic)
            args = { args };
        else if (sizeof...(TArgs) != args.size())
        {
            throw InternalError("CallbackImpl::invoke: wrong number of arguments");
            // throw std::runtime_error("wrong number of arguments");
        }

        std::tuple<typename std::remove_reference<TArgs>::type&...> tp = vec2tuple(args.begin(), (typename std::remove_reference<TArgs>::type*)nullptr...);
        return construct(applyTuple(this->m_fun, tp));
    }

    template <typename TRet, typename... TArgs>
    Signature CallbackImpl<TRet, TArgs...>::signature() const
    {
        if (this->m_variadic)
            return Signature(true);

        return Signature(explicit_pack2vec<Class::Id, typename always_of<Class::Id, TArgs>::type...>(type_class<TArgs>().classid()...), true);
    }

    template <typename... TArgs>
    Object CallbackImpl<void, TArgs...>::invoke(std::vector<Object> args)
    {
        if (this->m_variadic)
            args = { args };
        else  if (sizeof...(TArgs) != args.size())
        {
            throw InternalError("CallbackImpl::invoke: wrong number of arguments");
            // throw std::runtime_error("wrong number of arguments");
        }

        std::tuple<typename std::remove_reference<TArgs>::type&...> tp = vec2tuple(args.begin(), (typename std::remove_reference<TArgs>::type*) nullptr...);
        applyTuple(this->m_fun, tp);

        return Object::nil();
    }

    template <typename... TArgs>
    Signature CallbackImpl<void, TArgs...>::signature() const
    {
        if (this->m_variadic)
            return Signature(false);

        return Signature(explicit_pack2vec<Class::Id, typename always_of<Class::Id, TArgs>::type...>(type_class<TArgs>().classid()...), false);
    }
}

#endif // __AXOLOTL_CORE_CALLBACK_IMPL_H__
