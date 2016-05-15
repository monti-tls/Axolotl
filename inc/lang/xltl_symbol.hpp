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

#ifndef __AXOLOTL_LANG_XLTL_SYMBOL_H__
#define __AXOLOTL_LANG_XLTL_SYMBOL_H__

#include "core/core.hpp"

#include <string>

namespace lang
{
    class XltlSymbol
    {
    public:
        enum Which
        {
            None,
            Package,
            Auto,
            Argument
        };

        enum Binding
        {
            Global,
            Local
        };

    public:
        XltlSymbol(Which which = None, std::string const& name = "", core::Object const& data = core::Object::nil());
        ~XltlSymbol();

        Which which() const;
        Binding binding() const;
        void setBinding(Binding binding);
        std::string const& name() const;
        core::Object const& data() const;
        core::Object& data();

    private:
        Which m_which;
        Binding m_binding;
        std::string m_name;
        core::Object m_data;
    };
}

#endif // __AXOLOTL_LANG_XLTL_SYMBOL_H__
