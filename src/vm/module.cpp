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

#include "vm/module.hpp"
#include "vm/function.hpp"
#include "vm/engine.hpp"
#include "vm/import_table.hpp"
#include "lang/std_names.hpp"
#include "lang/lang.hpp"

#include <fstream>

using namespace vm;
using namespace bits;
using namespace core;
using namespace lang;

Module::Module()
    : m_impl(nullptr)
{}

Module::Module(Blob const& blob, ImportTable* import_table)
    : m_impl(new Impl())
{
    m_impl->refcount = 1;
    m_impl->blob = blob;
    m_impl->name = m_impl->blob.moduleName();
    m_impl->engine = nullptr;
    m_impl->init_called = false;

    if (import_table)
        m_impl->import_table = import_table;
    else
        m_impl->import_table = new ImportTable();
}

Module::Module(std::string const& name, ImportTable* import_table)
    : m_impl(new Impl())
{
    m_impl->refcount = 1;
    m_impl->name = name;
    m_impl->engine = nullptr;
    m_impl->init_called = false;

    if (import_table)
        m_impl->import_table = import_table;
    else
        m_impl->import_table = new ImportTable();
}

Module::Module(Module const& cpy)
    : m_impl(cpy.m_impl)
{ M_incref(); }

Module::~Module()
{ M_decref(); }

Module& Module::operator=(Module const& cpy)
{
    M_decref();
    m_impl = cpy.m_impl;
    M_incref();
    
    return *this;
}

bool Module::operator==(Module const& other) const
{ return m_impl == other.m_impl; }

std::string const& Module::name() const
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::name: access to empty module");
    }
    
    return m_impl->name;
}

Object& Module::global(std::string const& name)
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::global: access to empty module");
    }
    
    return m_impl->globals[name];
}

Object Module::global(std::string const& name) const
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::global: access to empty module");
    }
    
    auto it = m_impl->globals.find(name);
    if (it == m_impl->globals.end())
    {
        throw NoGlobalError(*this, name);
    }
    return it->second;
}

int Module::addConstant(Object value)
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::addConstant: access to empty module");
    }
    
    m_impl->constants.push_back(value);
    return (int) m_impl->constants.size() - 1;
}

Object Module::constant(int index) const
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::constant: access to empty module");
    }
    
    if (index < 0 || index >= (int) m_impl->constants.size())
    {
        throw core::InternalError("vm::Module::global: constant access out of bounds");
    }

    return m_impl->constants[index];
}

void Module::setBlob(Blob const& blob)
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::setBlob: access to empty module");
    }
    
    m_impl->blob = blob;
    m_impl->blob.setModuleName(m_impl->name);

    M_processSymbols();
    M_processTypeSpecs();
    M_processConstants();
}

Blob const& Module::blob() const
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::blob: access to empty module");
    }
    
    return m_impl->blob;
}

void Module::setEngine(Engine* engine)
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::setEngine: access to empty module");
    }
    
    m_impl->engine = engine;
}

Engine* Module::engine() const
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::engine: access to empty module");
    }
    
    return m_impl->engine;
}

ImportTable* Module::importTable()
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::importTable: access to empty module");
    }
    
    if (m_impl->import_table)
        return m_impl->import_table;
    else if (m_impl->engine)
        return m_impl->engine->importTable();

    return nullptr;
}

ImportTable* Module::detachImportTable()
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::detachImportTable: access to empty module");
    }
    
    ImportTable* table = m_impl->import_table;
    m_impl->import_table = nullptr;
    return table;
}

void Module::exportTo(Module& to, std::string const& mask, std::string const& alias, core::Object extra) const
{
    if (!m_impl || !to.m_impl)
    {
        throw core::InternalError("vm::Module::exportTo: access to empty module");
    }

    if (to.m_impl == m_impl)
    {
        throw core::InternalError("vm::Module::exportTo: can't export module to itself");
    }

    for (auto& it : m_impl->globals)
    {
        std::string sym_name = it.first;
        std::string ext_name;

        if (it.first == std_main)
            ext_name = alias + "." + sym_name;
        else
        {
            if (mask == std_package_wildcard)
                ext_name = sym_name;
            else if (!mask.size())
                ext_name = alias + "." + sym_name;
            else
            {
                if (sym_name != mask)
                    continue;

                ext_name = sym_name;
            }
        }

        // Don't check for clashes
        to.global(ext_name) = it.second;

        if (!extra.isNil())
            extra(ext_name);
    }
}

bool Module::initCalled() const
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::initCalled: access to empty module");
    }

    return m_impl->init_called;
}

void Module::init()
{
    if (!m_impl)
    {
        throw core::InternalError("vm::Module::init: access to empty module");
    }
    
    if (!initCalled())
    {
        auto it = m_impl->globals.find(std_main);
        if (it != m_impl->globals.end())
            it->second();
        m_impl->init_called = true;
    }
}

void Module::M_incref()
{
    if (m_impl)
    {
        ++m_impl->refcount;
        // std::cout << "[" << m_impl << "]++ " << m_impl->refcount << std::endl;
    }
}

void Module::M_decref()
{
    if (m_impl && !--m_impl->refcount)
    {
        // std::cout << "[" << m_impl << "]-- " << m_impl->refcount << std::endl;

        if (m_impl->import_table)
            delete m_impl->import_table;
        delete m_impl;
        m_impl = nullptr;
    }
}

void Module::M_processSymbols()
{
    m_impl->blob.foreachSymbol([&](blob_idx, blob_symbol* sym)
    {
        if (sym->s_bind == BLOB_SYMB_GLOBAL)
        {
            std::string name;
            if (!m_impl->blob.string(sym->s_name, name))
            {
                throw core::InternalError("vm::Module::M_processSymbols: invalid symbol");
            }

            if (m_impl->globals.find(name) != m_impl->globals.end())
            {
                throw core::InternalError("vm::Module::M_processSymbols: symbol \'" + name + "' redefined");
            }

            m_impl->globals[name] = M_makeFunction(sym);
        }
    });
}

void Module::M_processTypeSpecs()
{
    m_impl->blob.foreachTypeSpec([&](blob_idx tidx, blob_typespec* tspec)
    {
        std::string type_name;
        if (!m_impl->blob.string(tspec->ts_name, type_name))
        {
            throw core::InternalError("vm::Module::M_processTypeSpecs: invalid type specification");
        }

        Class c(m_impl->name, type_name, true);

        m_impl->blob.foreachTypeSpecSymbol(tidx, [&](blob_idx symidx)
        {
            blob_symbol* sym = m_impl->blob.symbol(symidx);

            std::string name;
            if (!sym || !m_impl->blob.string(sym->s_name, name) ||
                sym->s_type != BLOB_SYMT_METHOD)
            {
                throw core::InternalError("vm::Module::M_processTypeSpecs: invalid symbol");
            }

            c.addMember(name, M_makeFunction(sym));
        });
        
        m_impl->globals[type_name] = c;
    });
}

void Module::M_processConstants()
{
    m_impl->blob.foreachConstant([&](blob_idx, blob_constant* cst)
    {
        std::string serialized;
        if (!m_impl->blob.string(cst->c_serialized, serialized))
        {
            throw core::InternalError("vm::Module::M_processConstants: invalid constant");
        }
        
        addConstant(class_from_classid(cst->c_classid).unserialize(serialized));
    });
}

Object Module::M_makeFunction(blob_symbol* symbol) const
{ return Callable(Function(*this, symbol)); }
