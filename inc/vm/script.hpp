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

#ifndef __AXOLOTL_VM_SCRIPT_H__
#define __AXOLOTL_VM_SCRIPT_H__

#include "core/core.hpp"
#include "vm/forward.hpp"
#include "vm/module.hpp"
#include "vm/import_table.hpp"
#include "lang/std_names.hpp"

#include <iostream>
#include <string>

namespace vm
{
    class Script
    {
    public:
        Script();
        Script(std::istream& is);
        Script(std::string const& filename);
        ~Script();

        void addModule(Module const& module);
        Module module() const;

        void fromFile(std::string const& filename);
        void fromString(std::string const& str);
        void fromStream(std::istream& is);

        core::Object run(std::string const& function = lang::std_main);
        core::Object run(std::string const& function, std::vector<core::Object> const& args);

        template <typename... TArgs>
        core::Object run(std::string const& function, TArgs const&... args)
        { return run(function, core::pack2vec(args...)); }

    private:
        ImportTable* m_import_table;
        bool m_built;
        Module m_module;
        Engine* m_engine;
    };
}

#endif // __AXOLOTL_VM_SCRIPT_H__
