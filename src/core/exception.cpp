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

#include "core/exception.hpp"
#include "vm/module.hpp"

#include <sstream>

using namespace core;

Exception::~Exception()
{}

InternalError::InternalError(std::string const& desc)
    : m_desc(desc)
{}

InternalError::~InternalError()
{}

std::string InternalError::what() const
{ return "internal error: " + m_desc.size() ? m_desc : "???"; }

NoMemberError::NoMemberError(Object const& self, std::string const& name)
    : m_self(self)
    , m_name(name)
{}

NoMemberError::~NoMemberError()
{}

std::string NoMemberError::what() const
{
    std::ostringstream ss;
    ss << "instance of class `" << m_self.classname() << "' has no member named `" << m_name << "'";
    return ss.str();
}

ClassError::ClassError(Object const& self, Class const& expected)
    : m_self(self)
    , m_expected(expected)
{}

ClassError::~ClassError()
{}

std::string ClassError::what() const
{
    std::ostringstream ss;
    ss << "expected an instance of class `" << m_expected.classname() << "', got one of class `" << m_self.classname() << "'";
    return ss.str();
}

SignatureError::SignatureError(Object const& self, std::string const& name, std::vector<Object> const& argv)
    : m_self(self)
    , m_name(name)
    , m_argv(argv)
{}

SignatureError::~SignatureError()
{}

std::string SignatureError::what() const
{
    std::ostringstream ss;
    if (m_name.size())
    {
        ss << "no member named `" << m_name << "' in instance of class `" << m_self.classname() << "' matches the given signature";
    }
    else
    {
        ss << "direct object invocation signature does not match in instance of class `"  << m_self.classname() << "'";
    }
    return ss.str();
}

NoFileError::NoFileError(std::string const& filename)
    : m_filename(filename)
{}

NoFileError::~NoFileError()
{}

std::string NoFileError::what() const
{ return "file `" + m_filename + "' not found"; }

NoGlobalError::NoGlobalError(vm::Module const& module, std::string const& name)
    : m_module_name(module.name())
    , m_name(name)
{}

NoGlobalError::~NoGlobalError()
{}

std::string NoGlobalError::what() const
{
    std::ostringstream ss;
    ss << "no global named `" << m_name << "' in module `" << m_module_name << "'";
    return ss.str();
}
