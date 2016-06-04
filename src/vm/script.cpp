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

#include "vm/script.hpp"
#include "vm/engine.hpp"
#include "lang/lang.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace vm;
using namespace core;
using namespace lang;

Script::Script()
    : m_import_table(new ImportTable())
    , m_built(false)
    , m_engine(nullptr)
{}

Script::Script(std::istream& is)
    : m_import_table(new ImportTable())
    , m_built(false)
    , m_engine(nullptr)
{ fromStream(is); }

Script::Script(std::string const& filename)
    : m_import_table(new ImportTable())
    , m_built(false)
    , m_engine(nullptr)
{ fromFile(filename); }

Script::~Script()
{
    if (m_import_table)
        delete m_import_table;
    if (m_engine)
        delete m_engine;
}

void Script::addModule(Module const& module)
{
    if (m_built)
    {
        throw core::InternalError("vm::Script::addModule: script is already built");
    }

    m_import_table->addModule(module);
}

Module Script::module() const
{ return m_module; }

void Script::fromFile(std::string const& filename)
{
    std::ifstream fs(filename);
    if (!fs)
    {
        throw core::NoFileError(filename);
    }
    fromStream(fs);
}

void Script::fromString(std::string const& str)
{
    std::istringstream ss;
    ss.str(str);
    fromStream(ss);
}

void Script::fromStream(std::istream& is)
{
    if (m_built)
    {
        throw core::InternalError("vm::Script::fromStream: script is already built");
    }

    m_module = Module(std_main, m_import_table);
    m_import_table = nullptr;

    Compiler* compiler = new Compiler(m_module, is);
    Module module(compiler->compile());
    delete compiler;

    m_engine = new Engine(module);
    m_built = true;
}

core::Object Script::run(std::string const& function)
{ return m_module.global(function)(); }

core::Object Script::run(std::string const& function, std::vector<Object> const& args)
{ return m_module.global(function).invoke(args); }
