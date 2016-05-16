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

Module::Module(std::string const& name)
    : m_impl(new Impl())
{
    m_impl->refcount = 1;
    m_impl->name = name;
    m_impl->engine = nullptr;
}

Module::Module(std::string const& name, Blob const& blob)
    : m_impl(new Impl())
{
    m_impl->refcount = 1;
    m_impl->name = name;
    m_impl->engine = nullptr;

    setBlob(blob);
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
{ return m_impl->name; }

Object& Module::global(std::string const& name)
{ return m_impl->globals[name]; }

Object const& Module::global(std::string const& name) const
{
    auto it = m_impl->globals.find(name);
    if (it == m_impl->globals.end())
        throw std::runtime_error("vm::Module::global: global '" + name + "' does not exists");
    return it->second;
}

int Module::addConstant(Object const& value)
{
    m_impl->constants.push_back(value);
    return (int) m_impl->constants.size() - 1;
}

Object const& Module::constant(int index) const
{
    if (index < 0 || index >= (int) m_impl->constants.size())
        throw std::runtime_error("bm::Module::global: constant access out of bounds");

    return m_impl->constants[index];
}

void Module::setBlob(Blob const& blob)
{
    m_impl->blob = blob;

    M_processSymbols();
    M_processTypeSpecs();
    M_processConstants();
}

Blob const& Module::blob() const
{ return m_impl->blob; }

void Module::setEngine(Engine* engine)
{ m_impl->engine = engine; }

Engine* Module::engine() const
{ return m_impl->engine; }

std::list<Module> const& Module::imports() const
{
    if (m_impl->engine)
        return m_impl->engine->imports();

    return m_impl->imports;
}

Module Module::import(std::string const& name, std::string const& mask, lang::Symtab* symtab)
{
    std::string file = name + ".xl";
    std::ifstream ss(file);
    if (!ss)
        throw std::runtime_error("vm::Module::import: `" + file + "' not found");

    Module module;
    bool already_imported = false;
    for (auto const& m : m_impl->imports)
    {
        if (m.name() == name)
        {
            module = m;
            already_imported = true;
            break;
        }
    }

    if (!already_imported)
    {
        module = Compiler(name, ss).compile();
    }

    for (auto it : module.m_impl->globals)
    {
        std::string glob_name;

        if (it.first == std_main)
            glob_name = name + "." + it.first;
        else
        {
            if (mask == std_package_wildcard)
                glob_name = it.first;
            else if (!mask.size())
                glob_name = name + "." + it.first;
            else
            {
                if (it.first != mask)
                    continue;

                glob_name = it.first;
            }
        }

        // Don't check for clashes
        m_impl->globals[glob_name] = it.second;

        if (symtab)
        {
            Symbol symbol(Symbol::Auto, glob_name);
            // Don't check for clashes
            symtab->add(symbol);
        }
    }

    if (!m_impl->engine)
        m_impl->imports.push_back(module);
    return module;
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
    if (m_impl)
    {
        // std::cout << "[" << m_impl << "]-- " << m_impl->refcount-1 << std::endl;

        if (!(--m_impl->refcount))
        {
            delete m_impl;
            m_impl = nullptr;
        }
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
                throw std::runtime_error("vm::Module::M_processSymbols: invalid symbol");

            if (m_impl->globals.find(name) != m_impl->globals.end())
                throw std::runtime_error("vm::Module::M_processSymbols: symbol \'" + name + "' redefined");

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
            throw std::runtime_error("vm::Module::M_processTypeSpecs: invalid type specification");
        std::string iface_name = lang::std_iface_prefix + type_name;

        std::vector<std::pair<std::string, Object>> members;
        m_impl->blob.foreachTypeSpecSymbol(tidx, [&](blob_idx symidx)
        {
            blob_symbol* sym = m_impl->blob.symbol(symidx);

            std::string name;
            if (!sym || !m_impl->blob.string(sym->s_name, name) ||
                sym->s_type != BLOB_SYMT_METHOD)
                throw std::runtime_error("vm::Module::M_processTypeSpecs: invalid symbol");

            members.push_back(std::make_pair(name, M_makeFunction(sym)));
        });

        if (m_impl->globals.find(iface_name) != m_impl->globals.end())
            throw std::runtime_error("vm::Module::M_processTypeSpecs: iface '" + iface_name + "' redefined");

        m_impl->globals[iface_name] = std::function<Object()>([=]()
        {
            Object object(Object::Kind::Scalar, Some(), type_name);
            Object::setupBuiltinMembers(object);
            for (auto member : members)
                object.member(member.first) = member.second;
            return object;
        });
    });
}

void Module::M_processConstants()
{
    m_impl->blob.foreachConstant([&](blob_idx, blob_constant* cst)
    {
        std::string type;
        if (!m_impl->blob.string(cst->c_type, type))
            throw std::runtime_error("vm::Module::M_processConstants: invalid constant");

        std::string serialized;
        if (!m_impl->blob.string(cst->c_serialized, serialized))
            throw std::runtime_error("vm::Module::M_processConstants: invalid constant");
        
        addConstant(ObjectFactory::typeMethod(type, lang::std_unserialize).invoke({ serialized }));
    });
}

Object Module::M_makeFunction(blob_symbol* symbol) const
{ return Object(Object::Kind::Callable, Callable(Function(*this, symbol)), lang::std_callable_classname); }
