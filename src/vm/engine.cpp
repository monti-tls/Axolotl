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

#include "bits/bits.hpp"
#include "vm/engine.hpp"
#include "vm/function.hpp"
#include "lang/lexer.hpp"
#include "lang/parser_base.hpp"
#include "lang/std_names.hpp"
#include "util/ansi.hpp"

#include <memory>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>

using namespace vm;
using namespace bits;
using namespace core;

Engine::Engine(Module const& main_module)
    : m_main_module(main_module)
    , m_text(nullptr)
    , m_module(nullptr)
{
    m_import_table = m_main_module.detachImportTable();
    m_import_table->setEngine(this);

    m_main_module.setEngine(this);
    M_changeModule(main_module);

    M_initOpcodes();

    m_argc = 0;
    m_locals_count = 0;
    m_debug.has = false;
    m_pc = -1;
}

Engine::~Engine()
{
    m_import_table->setEngine(nullptr);
    m_main_module.setEngine(nullptr);

    if (m_import_table)
        delete m_import_table;

    if (m_module)
        delete m_module;
}

Object Engine::execute(Function const& fun, std::vector<Object> const& args)
{
    int tos = M_stackIndex();

    // Setup arguments
    for (int i = 0; i < (int) args.size(); ++i)
        M_push(args[i]);

    // Push a dummy stack frame, when we will
    //   reach it execution will stop
    M_enter(true);
    m_argc = (int) args.size();

    // Branch to the function
    M_branchToFunction(fun);

    while (M_execute());

    Object ret = Object::nil();
    if (M_stackIndex() != tos)
        ret = M_pop();

    return ret;
}

ImportTable* Engine::importTable() const
{ return m_import_table; }

void Engine::M_initOpcodes()
{
    #define DEF_MASK(name, value)
    #define DEF_OPCODE(name, nargs) m_opcodes_nargs[name] = nargs;
    #include "bits/opcodes.def"
    #undef DEF_OPCODE
    #undef DEF_MASK

    int nargs_max = 0;
    for (auto it : m_opcodes_nargs)
        if (it.second > nargs_max)
            nargs_max = it.second;
    m_operands.reserve(nargs_max);
}

bool Engine::M_checkOpcode(Opcode opcode) const
{ return m_opcodes_nargs.find(opcode) != m_opcodes_nargs.end(); }

Object& Engine::M_top()
{
    if (!m_stack.size())
    {
        throw InternalError("vm::Engine::M_top: stack is empty");
        // throw std::runtime_error("vm::Engine::M_top: stack is empty");
    }
    return m_stack.back();
}

Object Engine::M_top() const
{
    if (!m_stack.size())
    {
        throw InternalError("vm::Engine::M_top: stack is empty");
        // throw std::runtime_error("vm::Engine::M_top: stack is empty");
    }
    return m_stack.back();
}

Object Engine::M_pop()
{
    Object top = M_top();
    m_stack.pop_back();
    return top;
}

void Engine::M_push(Object value)
{ m_stack.push_back(value); }

void Engine::M_growStack(std::size_t amount)
{
    for (int i = 0; i < (int) amount; ++i)
        M_push(Object::nil());
}

void Engine::M_shrinkStack(std::size_t amount)
{
    for (int i = 0; i < (int) amount; ++i)
        M_pop();
}

int Engine::M_stackIndex() const
{ return (int) m_stack.size() - 1; }

core::Object& Engine::M_stackAt(int index)
{
    if (index < 0 || index >= (int) m_stack.size())
    {
        throw InternalError("vm::Engine::M_stackAt: invalid index");
        // throw std::runtime_error("vm::Engine::M_stackAt: invalid index");
    }
    return m_stack.at(index);
}

core::Object Engine::M_stackAt(int index) const
{
    if (index < 0 || index >= (int) m_stack.size())
    {
        throw InternalError("vm::Engine::M_stackAt: invalid index");
        // throw std::runtime_error("vm::Engine::M_stackAt: invalid index");
    }
    return m_stack.at(index);
}

void Engine::M_changeModule(Module const& module)
{
    if (m_module && module == *m_module)
        return;

    m_text = module.blob().text();
    if (!m_text)
    {
        throw InternalError("vm::Engine::M_changeModule: module has no text");
        // throw std::runtime_error("vm::Engine::M_changeModule: module has no text");
    }

    blob_debug_header* debug_header = module.blob().debugHeader();
    if (debug_header)
    {
        if (!module.blob().string(debug_header->d_file, m_debug.file))
        {
            throw InternalError("vm::Engine::M_changeModule: file name in debug header invalid");
            // throw std::runtime_error("vm::Engine::M_changeModule: file name in debug header invalid");
        }
    }

    if (m_module)
        delete m_module;
    m_module = new Module(module);
}

uint32_t Engine::M_fetch()
{
    if (!m_text)
    {
        throw InternalError("vm::Engine::M_fetch: no text");
        // throw std::runtime_error("vm::Engine::M_fetch: no text");
    }
    if (m_pc < 0 || m_pc >= (int) (m_text->size() / sizeof(uint32_t)))
    {
        throw InternalError("vm::Engine::M_fetch: invalid PC");
        // throw std::runtime_error("vm::Engine::M_fetch: invalid PC");
    }

    return *((uint32_t*) m_text->raw(sizeof(uint32_t) * m_pc++, sizeof(uint32_t)));
}

void Engine::M_decode()
{
    // Get the opcode
    m_ir = (Opcode) M_fetch();

    if (m_ir & DEBUG_MASK)
    {
        m_ir = (Opcode) (m_ir & (~DEBUG_MASK));
        uint32_t idx = M_fetch();

        blob_debug_entry* entry = m_module->blob().debugEntry(idx);
        if (!entry)
        {
            throw InternalError("vm::Engine::M_decode: invalid debug entry index");
            // M_error("M_decode: invalid debug entry index");
        }

        m_debug.has = true;
        m_debug.line = entry->de_line;
        m_debug.col = entry->de_col;
        m_debug.extent = entry->de_extent;
    }
    else
        m_debug.has = false;

    if (!M_checkOpcode(m_ir))
    {
        throw InternalError("vm::Engine::M_decode: invalid instruction opcode");
        // throw std::runtime_error("vm::Engine::M_decode: invalid instruction opcode");
    }

    // Get the operands
    m_operands.clear();
    for (int i = 0; i < m_opcodes_nargs[m_ir]; ++i)
        m_operands.push_back(M_fetch());
}

bool Engine::M_execute()
{
    try
    {
        M_decode();

        switch (m_ir)
        {
            case NOP:
                break;

            case POP:
                M_pop();
                break;

            case LOAD_LOCAL:
            case STOR_LOCAL:
            {
                int index = m_operands[0];

                // Check operands
                if (index < 0 ||
                    index >= m_locals_count)
                {
                    throw InternalError("vm::Engine::M_execute: invalid local index");
                    // M_error("M_execute: invalid local index");
                }

                // Get the local
                Object& local = M_stackAt(m_locals_start + index);

                if (m_ir == LOAD_LOCAL)
                    M_push(local);
                else // if (m_ir == STOR_LOCAL)
                    local = M_pop();

                break;
            }

            case LOAD_CONST:
            {
                int index = m_operands[0];

                // Load a constant from the consts table
                if (index >= 0)
                {
                    M_push(m_module->constant(index).copy());
                }
                // Load an argument
                else
                {
                    // local #n
                    //  ...
                    // local #0 <-- locals_start
                    // <frame>
                    // arg #n
                    // ...
                    // arg #i   <-- locals_start - 1 - argc + i
                    // ...
                    // arg #0
                    //
                    // We have i = 1 - index, so
                    //   the argument index in the stack is
                    //   locals_start - 1 - argc + 1 - index
                    //   = locals_start - argc - index (with index < 0)

                    M_push(M_stackAt(m_locals_start - m_argc - index-2));
                }

                break;
            }

            case LOAD_GLOBAL:
            case STOR_GLOBAL:
            {
                // Get and check the operand
                std::string name;
                if (!m_module->blob().string(m_operands[0], name))
                {
                    throw InternalError("vm::Engine::M_execute: invalid string index");
                    // M_error("M_execute: invalid string index");
                }

                // Get the global
                Object& global = m_module->global(name);

                if (m_ir == LOAD_GLOBAL)
                    M_push(global);
                else // if (m_ir == STOR_GLOBAL)
                    global = M_pop();

                break;
            }

            case LOAD_MEMBER:
            case STOR_MEMBER:
            {
                // Get and check the operand
                std::string name;
                if (!m_module->blob().string(m_operands[0], name))
                {
                    throw InternalError("vm::Engine::M_execute: invalid string index");
                    // M_error("M_execute: invalid string index");
                }

                // Get the object
                Object self = M_pop();

                if (m_ir == LOAD_MEMBER)
                {
                    M_push(((Object) self).member(name));
                }
                else // if (m_ir == STOR_MEMBER)
                {
                    self.member(name) = M_pop();
                }

                break;
            }

            case INVOKE:
            {
                int argc = m_operands[0];

                // Check operand
                if (argc < 0)
                {
                    throw InternalError("vm::Engine::M_execute: invalid operand");
                    // M_error("M_execute: invalid operand");
                }

                M_invoke(M_pop(), argc);
                break;
            }

            case METHOD:
            {
                std::string name;
                if (!m_module->blob().string(m_operands[0], name))
                {
                    throw InternalError("vm::Engine::M_execute: invalid string index");
                    // M_error("M_execute: invalid string index");
                }

                int argc = m_operands[1];
                if (argc < 0)
                {
                    throw InternalError("vm::Engine::M_execute: invalid operand");
                    // M_error("M_execute: invalid operand");
                }

                Object self = M_pop();
                if (!self.has(name))
                {
                    throw NoMemberError(self, name);
                    // M_error("M_execute: method '" + name + "' does not exists for class '" + self.classname() + "'");
                }

                // Insert the 'self' object on the stack
                std::vector<Object> argv(argc + 1, Object::nil());
                argv[0] = self;
                for (int i = 0; i < argc; ++i)
                    argv[argc - i] = M_pop();

                Object fun = self.findPolymorphic(name, argv);
                if (fun.isNil())
                {
                    throw SignatureError(self, name, argv);
                }

                for (auto arg : argv)
                    M_push(arg);

                M_invoke(fun, argc + 1);

                break;
            }

            case RETURN:
            case LEAVE:
            {
                Object ret = Object::nil();

                if (m_ir == RETURN)
                    ret = M_pop();

                bool dummy = M_leave();

                M_push(ret);
                
                if (dummy)
                    return false;

                break;
            }

            case JMP:
            case JMPR:
            {
                int base = 0;
                if (m_ir == JMPR)
                    base = m_pc;

                m_pc = base + m_operands[0];

                break;
            }

            case JMP_IF_FALSE:
            case JMPR_IF_FALSE:
            {
                Object cond = M_pop();
                if (!cond.meta().is<bool>())
                {
                    throw ClassError(cond, type_class<bool>());
                    // M_error("M_execute: conditional is not a boolean");
                }

                if (!cond.unwrap<bool>())
                {
                    int base = 0;
                    if (m_ir == JMPR_IF_FALSE)
                        base = m_pc;

                    m_pc = base + m_operands[0];
                }

                break;
            }

            case JMP_IF_TRUE:
            case JMPR_IF_TRUE:
            {
                Object cond = M_pop();
                if (!cond.meta().is<bool>())
                {
                    throw ClassError(cond, type_class<bool>());
                    // M_error("M_execute: conditional is not a boolean");
                }

                if (cond.unwrap<bool>())
                {
                    int base = 0;
                    if (m_ir == JMPR_IF_TRUE)
                        base = m_pc;

                    m_pc = base + m_operands[0];
                }

                break;
            }

            case IMPORT:
            {
                std::string name;
                if (!m_module->blob().string(m_operands[0], name))
                {
                    throw InternalError("vm::Engine::M_execute: invalid string index");
                    // M_error("M_execute: invalid string index");
                }

                Module module;

                if (!m_import_table->exists(name))
                {
                    module = m_import_table->import(*m_module, name);
                    module.setEngine(this);
                }
                else
                    module = m_import_table->module(name);

                if (!module.initCalled())
                    module.init();

                break;
            }

            case IMPORT_MASK:
            {
                std::string name;
                if (!m_module->blob().string(m_operands[0], name))
                {
                    throw InternalError("vm::Engine::M_execute: invalid string index");
                    // M_error("M_execute: invalid string index");
                }

                std::string mask;
                if (!m_module->blob().string(m_operands[1], mask))
                {
                    throw InternalError("vm::Engine::M_execute: invalid string index");
                    // M_error("M_execute: invalid string index");
                }

                Module module;

                if (!m_import_table->exists(name))
                {
                    module = m_import_table->importMask(*m_module, name, mask);
                    module.setEngine(this);
                }
                else
                    module = m_import_table->module(name);

                if (!module.initCalled())
                    module.init();

                break;
            }

            default:
                throw InternalError("vm::Engine::M_execute: invalid opcode");
                // M_error("M_execute: invalid opcode");
                break;
        }

        return true;
    }
    catch (Exception const& error)
    {
        M_error(error.what());
        return false;
    }
    catch (std::exception const& error)
    {
        M_error(error.what());
        return false;
    }
}

StackFrame Engine::M_makeFrame(bool dummy) const
{
    StackFrame frame;

    if (m_module)
        frame.module = *m_module;

    frame.dummy = dummy;
    frame.pc = m_pc;
    frame.locals_start = m_locals_start;
    frame.locals_count = m_locals_count;
    frame.argc = m_argc;
    if (m_debug.has)
        frame.debug = m_debug;

    return frame;
}

bool Engine::M_setFrame(StackFrame const& frame)
{
    M_changeModule(frame.module);
    m_pc = frame.pc;
    m_locals_start = frame.locals_start;
    m_locals_count = frame.locals_count;
    m_argc = frame.argc;

    return frame.dummy;
}

void Engine::M_invoke(Object fun, int argc)
{
    // We must get an argument vector to do polymorphic selection
    std::vector<Object> argv(argc, Object::nil());
    for (int i = 0; i < argc; ++i)
        argv[argc - i - 1] = M_pop();

    if (fun.invokable() && !fun.callable())
        argv.insert(argv.begin(), fun);

    while (fun.invokable() && !fun.callable())
        fun = fun.findPolymorphic(lang::std_call, argv);

    if (!fun.callable())
    {
        throw SignatureError(fun, "", argv);
        // M_error("M_invoke: object is not invokable");
    }

    Callable call = fun.unwrap<Callable>();

    if (!call.signature().match(argv))
    {
        throw SignatureError(fun, "", argv);
        // M_error("signature mismatch");
    }

    if (call.kind() == Callable::Kind::Native)
    {
        Object ret = call.invoke(argv);
        M_push(ret);
    }
    else
    {
        for (auto arg : argv)
            M_push(arg);

        M_enter();
        M_branchToFunction(call.meta().as<Function>());
        m_argc = argc;
    }
}

void Engine::M_enter(bool dummy)
{
    M_push(M_makeFrame(dummy));
    m_backtrace.push_back(M_stackIndex());
}

bool Engine::M_leave()
{
    m_backtrace.pop_back();

    M_shrinkStack(m_locals_count);
    
    Object frame = M_pop();
    if (!frame.meta().is<StackFrame>())
    {
        throw InternalError("vm::Engine::M_leave: stack was smashed and no stack frame was found");
        // M_error("M_leave: can't find the call frame");
    }

    int argc = m_argc;
    bool dummy = M_setFrame(frame.unwrap<StackFrame>());
    M_shrinkStack(argc);

    return dummy;
}

void Engine::M_branchToFunction(Function const& fun)
{
    M_changeModule(fun.module());
    m_pc = fun.symbol()->s_addr;
    m_locals_start = M_stackIndex() + 1;
    m_locals_count = fun.symbol()->s_nlocals;
    M_growStack(m_locals_count);
}

void Engine::M_error(std::string const& msg) const
{
    std::ostringstream ss;

    std::string prefix = "";
    if (m_debug.has)
    {
        std::ostringstream ss;
        ss << util::ansi::bold << m_debug.file << ":" << m_debug.line << ":" << m_debug.col << ": ";
        ss << util::ansi::clear;
        prefix = ss.str();
    }

    ss << prefix << util::ansi::bold << lang::ParserBase::error_color;
    ss << "runtime error: " << util::ansi::clear;
    ss << msg << std::endl;

    if (m_debug.has)
    {
        std::ifstream is(m_debug.file);
        if (is)
        {
            std::size_t pos;
            std::string line = lang::Lexer::snippet(is, m_debug.line, m_debug.col, pos);

            if (line.size())
            {
                std::string fmt = lang::ParserBase::emph_color;
                line.insert(pos, fmt);
                std::size_t end = std::min(line.size()-1, pos + fmt.size() + m_debug.extent);
                line.insert(end, util::ansi::clear);
            }

            pos += 4;
            ss << "    " << line << std::endl;

            for (int i = 0; i < ((int) pos); ++i)
                ss << " ";
            ss << lang::ParserBase::emph_color << "^";

            for (int i = 0; i < ((int) m_debug.extent) - 1; ++i)
                ss << "~";
            ss << util::ansi::clear << std::endl;
        }
    }

    ss << prefix << util::ansi::bold << lang::ParserBase::note_color;
    ss << "backtrace: " << util::ansi::clear << std::endl;

    for (int i = m_backtrace.size() - 1; i >= 0; --i)
    {
        std::ostringstream ss2;

        Object obj = M_stackAt(m_backtrace[i]);

        if (!obj.meta().is<StackFrame>())
            ss2 << "< " << util::ansi::bold << lang::ParserBase::error_color << "bogus" << util::ansi::clear << " >" << std::endl;
        else
        {
            StackFrame frame = obj.unwrap<StackFrame>();

            ss2 << util::ansi::bold << frame.module.name() << ".";

            bits::Disassembler dis(frame.module.blob());
            std::string name;
            int offset;
            if (dis.functionAt(frame.pc, name, offset))
            {
                ss2 << name << util::ansi::clear << " + " << offset;
            }
            else
                ss2 << "???" << util::ansi::clear;

            if (frame.debug.is<DebugInfo>())
            {
                DebugInfo info = frame.debug.as<DebugInfo>();

                std::ifstream is(info.file);
                if (is)
                {
                    std::string previous = ss2.str();
                    ss2.str("");

                    ss2 << std::setw(56) << std::left << previous << std::right << " ";

                    std::size_t pos;
                    std::string line = lang::Lexer::snippet(is, info.line, info.col, pos);
                    std::string fmt = lang::ParserBase::emph_color;
                    line.insert(pos, fmt);
                    std::size_t end = std::min(line.size()-1, pos + fmt.size() + info.extent);
                    line.insert(end, util::ansi::clear);
                    ss2 << "        " << util::ansi::bold << info.file << ":" << info.line << ":" << info.col << ": ";
                    ss2 << util::ansi::clear << line << std::endl;
                }
            }
            else
                ss2 << std::endl;
        }

        ss << ss2.str();
    }

    std::cerr << ss.str();
    std::terminate();
}
