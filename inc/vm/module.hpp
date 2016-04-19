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

#ifndef __AXOLOTL_VM_MODULE_H__
#define __AXOLOTL_VM_MODULE_H__

#include "vm/forward.hpp"
#include "bits/bits.hpp"
#include "core/core.hpp"

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace vm
{
    class Module
    {
    public:
        Module(bits::Blob const& blob);
        ~Module();

        core::Object& global(std::string const& name);
        core::Object const& global(std::string const& name) const;

        int addConstant(core::Object const& value);
        core::Object const& constant(int index) const;

        bits::Blob const& blob() const;

        void setEngine(Engine* engine);
        Engine* engine() const;

    private:
        void M_processSymbols();
        void M_processTypeSpecs();
        void M_processConstants();
        core::Object M_makeFunction(bits::blob_symbol* symbol) const;

    public:
        bits::Blob m_blob;
        std::map<std::string, core::Object> m_globals;
        std::vector<core::Object> m_constants;
        Engine* m_engine;
    };
}

#endif // __AXOLOTL_VM_MODULE_H__
