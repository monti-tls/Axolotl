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

#include "vm/import_table.hpp"
#include "vm/engine.hpp"
#include "lang/std_names.hpp"
#include "lang/lang.hpp"

#include <fstream>

using namespace vm;
using namespace lang;

std::map<std::string, Module> ImportTable::m_builtins;

ImportTable::ImportTable()
{
    m_symtab = nullptr;

    for (auto m : m_builtins)
        m_scope[m.first] = m.second;
}

ImportTable::~ImportTable()
{ }

void ImportTable::attachSymtab(Symtab* symtab)
{ m_symtab = symtab; }

Symtab* ImportTable::detachSymtab()
{
    Symtab* symtab = m_symtab;
    m_symtab = nullptr;
    return symtab;
}

void ImportTable::addModule(Module const& module)
{
    auto it = m_scope.find(module.name());
    if (it != m_scope.end())
        throw std::runtime_error("vm::ImportTable::addModule: module already in scope");

    m_scope[module.name()] = module;
}

Module ImportTable::import(Module& to, std::string const& name)
{ return M_import(to, name, "", name); }

Module ImportTable::importMask(Module& to, std::string const& name, std::string const& mask)
{ return M_import(to, name, mask, name); }

Module ImportTable::importAs(Module& to, std::string const& name, std::string const& alias)
{ return M_import(to, name, "", alias); }

bool ImportTable::exists(std::string const& name) const
{ return m_table.find(name) != m_table.end(); }

Module const& ImportTable::module(std::string const& name) const
{
    if (!exists(name))
        throw std::runtime_error("vm::ImportTable::module: module does not exists");

    return m_table.find(name)->second.module;
}

void ImportTable::setEngine(Engine* engine)
{
    for (auto& import : m_table)
        import.second.module.setEngine(engine);
}

Module ImportTable::M_import(Module& to, std::string const& name, std::string const& mask, std::string const& alias)
{
    Import import = M_open(name, alias);

    core::Object extra = [=](std::string const& ext_name)
    {
        if (m_symtab)
        {
            Symbol symbol(Symbol::Variable, Symbol::Auto, ext_name);
            m_symtab->add(symbol);
        }   
    };

    import.module.exportTo(to, mask, alias, extra);

    return import.module;
}

ImportTable::Import ImportTable::M_open(std::string const& name, std::string const& alias)
{
    // Check if the module was already imported
    if (exists(name))
        return m_table[name];

    // Check in current scope
    auto it = m_scope.find(name);
    if (it != m_scope.end())
    {
        Import import;
        import.alias = alias;
        import.module = it->second;
        m_table[name] = import;

        return m_table[name];        
    }

    // If we reach here, it's a user-defined module
    std::string file = name + ".xl";
    std::ifstream ss(file);
    if (!ss)
        throw std::runtime_error("vm::Module::import: `" + file + "' not found");

    Import import;
    import.alias = alias;
    import.module = Compiler(name, ss).compile();
    m_table[name] = import;

    return m_table[name];
}

void ImportTable::addBuiltin(Module const& module)
{
    auto it = m_builtins.find(module.name());
    if (it != m_builtins.end())
        throw std::runtime_error("vm::ImportTable::addBuiltin: module is already added");

    m_builtins[module.name()] = module;
}
