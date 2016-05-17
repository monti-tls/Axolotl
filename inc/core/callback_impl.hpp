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

#include <stdexcept>

namespace core
{
    template <typename TRet, typename... TArgs>
    Object CallbackImpl<TRet, TArgs...>::invoke(std::vector<Object> const& args)
    {
        if (sizeof...(TArgs) != args.size())
            throw std::runtime_error("wrong number of arguments");
        
        std::vector<Object>& writeable = const_cast<std::vector<Object>&>(args);
        std::tuple<typename std::remove_reference<TArgs>::type&...> tp = vec2tuple(writeable.begin(), (typename std::remove_reference<TArgs>::type*)nullptr...);

        return ObjectFactory::construct(applyTuple(this->m_fun, tp));
    }

    template <typename TRet, typename... TArgs>
    Signature CallbackImpl<TRet, TArgs...>::signature() const
    { return Signature(explicit_pack2vec<std::string, typename always_string<TArgs>::type...>(ObjectFactory::typeName<TArgs>()...), true); }

    template <typename... TArgs>
    Object CallbackImpl<void, TArgs...>::invoke(std::vector<Object> const& args)
    {
        if (sizeof...(TArgs) != args.size())
            throw std::runtime_error("wrong number of arguments");
        
        std::vector<Object>& writeable = const_cast<std::vector<Object>&>(args);
        std::tuple<typename std::remove_reference<TArgs>::type&...> tp = vec2tuple(writeable.begin(), (typename std::remove_reference<TArgs>::type*) nullptr...);
        applyTuple(this->m_fun, tp);

        return Object::nil();
    }

    template <typename... TArgs>
    Signature CallbackImpl<void, TArgs...>::signature() const
    { return Signature(explicit_pack2vec<std::string, typename always_string<TArgs>::type...>(ObjectFactory::typeName<TArgs>()...), false); }
}

#endif // __AXOLOTL_CORE_CALLBACK_IMPL_H__
