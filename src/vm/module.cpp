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
#include "lang/std_names.hpp"

using namespace vm;
using namespace bits;
using namespace core;

Module::Module(Blob const& blob)
    : m_blob(blob)
    , m_engine(nullptr)
{
    M_processSymbols();
    M_processTypeSpecs();
    M_processConstants();
}

Module::~Module()
{}

core::Object& Module::global(std::string const& name)
{ return m_globals[name]; }

core::Object const& Module::global(std::string const& name) const
{
    auto it = m_globals.find(name);
    if (it == m_globals.end())
        throw std::runtime_error("vm::Module::global: global '" + name + "' does not exists");
    return it->second;
}

int Module::addConstant(core::Object const& value)
{
    m_constants.push_back(value);
    return (int) m_constants.size() - 1;
}

core::Object const& Module::constant(int index) const
{
    if (index < 0 || index >= (int) m_constants.size())
        throw std::runtime_error("bm::Module::global: constant access out of bounds");

    return m_constants[index];
}

bits::Blob const& Module::blob() const
{ return m_blob; }

void Module::setEngine(Engine* engine)
{ m_engine = engine; }

Engine* Module::engine() const
{ return m_engine; }

void Module::M_processSymbols()
{
    m_blob.foreachSymbol([&](blob_idx, blob_symbol* sym)
    {
        if (sym->s_bind == BLOB_SYMB_GLOBAL)
        {
            std::string name;
            if (!m_blob.string(sym->s_name, name))
                throw std::runtime_error("vm::Module::M_processSymbols: invalid symbol");

            if (m_globals.find(name) != m_globals.end())
                throw std::runtime_error("vm::Module::M_processSymbols: symbol \'" + name + "' redefined");

            m_globals[name] = M_makeFunction(sym);
        }
    });
}

void Module::M_processTypeSpecs()
{
    m_blob.foreachTypeSpec([&](blob_idx tidx, blob_typespec* tspec)
    {
        std::string type_name;
        if (!m_blob.string(tspec->ts_name, type_name))
            throw std::runtime_error("vm::Module::M_processTypeSpecs: invalid type specification");
        std::string iface_name = lang::std_iface_prefix + type_name;

        std::vector<std::pair<std::string, Object>> members;
        m_blob.foreachTypeSpecSymbol(tidx, [&](blob_idx symidx)
        {
            blob_symbol* sym = m_blob.symbol(symidx);

            std::string name;
            if (!sym || !m_blob.string(sym->s_name, name) ||
                sym->s_type != BLOB_SYMT_METHOD)
                throw std::runtime_error("vm::Module::M_processTypeSpecs: invalid symbol");

            members.push_back(std::make_pair(name, M_makeFunction(sym)));
        });

        if (m_globals.find(iface_name) != m_globals.end())
            throw std::runtime_error("vm::Module::M_processTypeSpecs: iface '" + iface_name + "' redefined");

        m_globals[iface_name] = std::function<Object()>([=]()
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
    m_blob.foreachConstant([&](blob_idx, blob_constant* cst)
    {
        std::string type;
        if (!m_blob.string(cst->c_type, type))
            throw std::runtime_error("vm::Module::M_processConstants: invalid constant");

        std::string serialized;
        if (!m_blob.string(cst->c_serialized, serialized))
            throw std::runtime_error("vm::Module::M_processConstants: invalid constant");
        
        addConstant(ObjectFactory::typeMember(type, lang::std_unserialize).invoke({ serialized }));
    });
}

Object Module::M_makeFunction(bits::blob_symbol* symbol) const
{ return Object(Object::Kind::Callable, Callable(Function(const_cast<Module*>(this), symbol)), lang::std_callable_classname); }
