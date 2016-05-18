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

#ifndef __AXOLOTL_CORE_SIGNATURE_H__
#define __AXOLOTL_CORE_SIGNATURE_H__

#include "core/object.hpp"
#include "core/class.hpp"

#include <string>
#include <vector>

namespace core
{
    class Signature
    {
    public:
        typedef std::vector<Class::ClassId> TypeList;

    public:
        //! \param returns Only meaningful for C++ functions that can return void
        Signature(TypeList const& arguments, bool returns = false);
        ~Signature();

        bool returns() const;
        TypeList const& arguments() const;
        bool match(std::vector<Object> const& args) const;

    private:
        bool m_returns;
        TypeList m_arguments;
    };
}

#endif // __AXOLOTL_CORE_SIGNATURE_H__
