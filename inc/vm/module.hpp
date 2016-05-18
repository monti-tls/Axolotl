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
#include "lang/forward.hpp"

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <list>

namespace vm
{
    class Module
    {
    public:
        Module();
        Module(bits::Blob const& blob, ImportTable* import_table = nullptr);
        Module(std::string const& name, ImportTable* import_table = nullptr);
        Module(Module const& cpy);
        ~Module();

        Module& operator=(Module const& cpy);
        bool operator==(Module const& other) const;

        std::string const& name() const;

        core::Object& global(std::string const& name);
        core::Object const& global(std::string const& name) const;

        int addConstant(core::Object const& value);
        core::Object const& constant(int index) const;

        void setBlob(bits::Blob const& blob);
        bits::Blob const& blob() const;

        void setEngine(Engine* engine);
        Engine* engine() const;

        ImportTable* importTable();
        ImportTable* detachImportTable();

        void exportTo(Module& to, std::string const& mask = "", std::string const& alias = "",
                      core::Object const& extra = core::Object::nil()) const;

        bool initCalled() const;
        void init();

    private:
        void M_incref();
        void M_decref();
        void M_processSymbols();
        void M_processTypeSpecs();
        void M_processConstants();
        core::Object M_makeFunction(bits::blob_symbol* symbol) const;

    public:
        class Impl
        {
        public:
            std::string name;
            bits::Blob blob;
            std::map<std::string, core::Object> globals;
            std::vector<core::Object> constants;
            Engine* engine;
            ImportTable* import_table;
            bool init_called;
            int refcount;
        }* m_impl;
    };
}

#endif // __AXOLOTL_VM_MODULE_H__
