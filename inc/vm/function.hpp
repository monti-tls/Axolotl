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

#ifndef __AXOLOTL_VM_FUNCTION_H__
#define __AXOLOTL_VM_FUNCTION_H__

#include "vm/module.hpp"
#include "core/core.hpp"

#include <vector>
#include <memory>

namespace vm
{
    class Function
    {
    public:
        Function(Module const& module, bits::blob_symbol* symbol);
        ~Function();

        core::Object invoke(std::vector<core::Object> const& args) const;

        core::Signature const& signature() const;
        Module const& module() const;
        bits::blob_symbol* symbol() const;

    private:
        void M_createSignature();

    private:
        Module m_module;
        bits::blob_symbol* m_symbol;
        std::shared_ptr<core::Signature> m_signature;
    };
}

#endif // __AXOLOTL_VM_FUNCTION_H__
