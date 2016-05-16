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

#include "vm/function.hpp"
#include "vm/engine.hpp"

#include <string>
#include <stdexcept>

using namespace vm;
using namespace bits;
using namespace core;

Function::Function(Module const& module, bits::blob_symbol* symbol)
    : m_module(module)
    , m_symbol(symbol)
{
    M_createSignature();
}

Function::~Function()
{}

Object Function::invoke(std::vector<Object> const& args) const
{
    if (!m_module.engine())
        throw std::runtime_error("vm::Function::invoke: module is not attached to an engine");
    return m_module.engine()->execute(*this, args);
}

core::Signature const& Function::signature() const
{ return *m_signature; }

Module const& Function::module() const
{ return m_module; }

bits::blob_symbol* Function::symbol() const
{ return m_symbol; }

void Function::M_createSignature()
{
    std::vector<Signature::TypeName> args;
    m_module.blob().foreachSignatureArgument(m_symbol->s_signature, [&](blob_off soff)
    {
        std::string type;
        if (!m_module.blob().string(soff, type))
            throw std::runtime_error("vm::Function::M_createSignature: invalid signature argument");
        args.push_back(type);
    });
    m_signature = std::shared_ptr<Signature>(new Signature(args));
}
