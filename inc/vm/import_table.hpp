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

#ifndef __AXOLOTL_VM_IMPORT_TABLE_H__
#define __AXOLOTL_VM_IMPORT_TABLE_H__

#include "lang/forward.hpp"
#include "vm/forward.hpp"
#include "vm/module.hpp"

#include <string>
#include <map>

namespace vm
{
    class ImportTable
    {
    private:
        class Import
        {
        public:
            std::string alias;
            bool init_called;
            Module module;
        };

    public:
        ImportTable();
        ~ImportTable();

        void attachSymtab(lang::Symtab* symtab);
        lang::Symtab* detachSymtab();

        void addModule(Module const& module);

        Module import(Module& to, std::string const& name);
        Module importMask(Module& to, std::string const& name, std::string const& mask);
        Module importAs(Module& to, std::string const& name, std::string const& alias);

        bool exists(std::string const& name) const;
        Module const& module(std::string const& name) const;

        void setEngine(Engine* engine);

    private:
        Module M_import(Module& to, std::string const& name, std::string const& alias, std::string const& mask);
        Import M_open(std::string const& name, std::string const& alias);

    private:
        lang::Symtab* m_symtab;
        std::map<std::string, Module> m_scope;
        std::map<std::string, Import> m_table;

    public:
        static void addBuiltin(Module const& module);

    private:
        static std::map<std::string, Module> m_builtins;
    };
}

#endif // __AXOLOTL_VM_IMPORT_TABLE_H__
