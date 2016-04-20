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

#ifndef __AXOLOTL_CORE_OBJECT_IMPL_H__
#define __AXOLOTL_CORE_OBJECT_IMPL_H__

#include "core/object.hpp"
#include "core/object_factory.hpp"
#include "core/ppack.hpp"

namespace core
{
	template <typename T>
	Object::Object(T value)
        : m_impl(nullptr)
	{ *this = ObjectFactory::build(value); }

    template <typename T>
    T& Object::unwrap()
    {
        if (!m_impl->meta.is<T>())
            throw std::runtime_error("attempt to unwrap a object of incompatible type");

        return m_impl->meta.as<T>();
    }

    template <typename T>
    T const& Object::unwrap() const
    {
        if (!m_impl->meta.is<T>())
            throw std::runtime_error("attempt to unwrap a object of incompatible type");

        return m_impl->meta.as<T>();
    }

    template <typename... Args>
    Object Object::operator()(Args const&... args) const
    { return invoke(pack2vec(args...)); }
}

#endif // __AXOLOTL_CORE_OBJECT_IMPL_H__
