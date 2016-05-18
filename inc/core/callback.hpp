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

#ifndef __AXOLOTL_CORE_CALLBACK_H__
#define __AXOLOTL_CORE_CALLBACK_H__

#include "core/object.hpp"
#include "core/ppack.hpp"
#include "core/signature.hpp"

#include <vector>
#include <string>
#include <functional>
#include <cstddef>
#include <memory>

namespace core
{
    class AbstractCallback
    {
    public:
        AbstractCallback()
        {
        }
        virtual ~AbstractCallback()
        {
        }

        virtual Object invoke(std::vector<Object> const& args) = 0;
        virtual Signature signature() const = 0;
    };

    template <typename TRet, typename... TArgs>
    class CallbackBase : public AbstractCallback
    {
    public:
        CallbackBase(std::function<TRet(TArgs...)> const& fun, bool variadic = false)
            : m_fun(fun)
            , m_variadic(variadic)
        {
        }

        ~CallbackBase()
        {
        }

    protected:
        std::function<TRet(TArgs...)> m_fun;
        bool m_variadic;
    };

    template <typename TRet, typename... TArgs>
    class CallbackImpl : public CallbackBase<TRet, TArgs...>
    {
    public:
        using CallbackBase<TRet, TArgs...>::CallbackBase;

        Object invoke(std::vector<Object> const& args);
        Signature signature() const;
    };

    template <typename... TArgs>
    class CallbackImpl<void, TArgs...> : public CallbackBase<void, TArgs...>
    {
    public:
        using CallbackBase<void, TArgs...>::CallbackBase;

        Object invoke(std::vector<Object> const& args);
        Signature signature() const;
    };
}

#endif // __AXOLOTL_CORE_CALLBACK_H__
