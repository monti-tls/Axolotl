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

#ifndef __AXOLOTL_CORE_CALLABLE_H__
#define __AXOLOTL_CORE_CALLABLE_H__

#include "core/some.hpp"
#include "core/object.hpp"
#include "core/callback.hpp"
#include "core/signature.hpp"
#include "bits/forward.hpp"
#include "vm/forward.hpp"

#include <functional>
#include <vector>
#include <memory>

namespace core
{
    //! This class encapsulates an object which can be called.
    //! It is used internally by the core::Object class.
    class Callable
    {
    public:
        //! Nature of the callable object
        enum class Kind
        {
            //! The object is a native function (i.e, exposed from C++)
            Native,
            //! The object is a function defined by a script
            Scripted
        };

        //! Type of the internal data for Native type
        typedef std::shared_ptr<AbstractCallback> NativeMetaType;
        //! Type of the internal data for the Scripted type
        typedef vm::Function ScriptedMetaType;

    public:
        //! Build a scripted callable object
        Callable(ScriptedMetaType const& scripted);

        //! Build a native callable object from a std::function
        template <typename TRet, typename... TArgs>
        Callable(std::function<TRet(TArgs...)> const& fun)
            : m_kind(Kind::Native)
            , m_meta(NativeMetaType(new CallbackImpl<TRet, TArgs...>(fun)))
        {}

        ~Callable();

        //! Get the kind of this callable
        Kind kind() const;
        //! Get the internal data of this callabel
        Some const& meta() const;

        //! Invoke this callable using the provided argument vector
        Object invoke(std::vector<Object> const& args) const;
        //! Get the signature of this callable
        Signature signature() const;

    private:
        Object M_nativeInvocation(std::vector<Object> const& args) const;
        Object M_scriptedInvocation(std::vector<Object> const& args) const;

    private:
        Kind m_kind;
        Some m_meta;
    };
}

#endif // __AXOLOTL_CORE_CALLABLE_H__
