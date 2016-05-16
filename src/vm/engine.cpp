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
#include "lang/std_names.hpp"

#include <memory>
#include <stdexcept>
#include <iostream>

using namespace vm;
using namespace bits;
using namespace core;

Engine::Engine(Module const& main_module)
    : m_main_module(main_module)
    , m_text(nullptr)
    , m_module(nullptr)
{
    m_imports = m_main_module.imports();
    for (auto& module : m_imports)
        module.setEngine(this);

    m_main_module.setEngine(this);
    M_changeModule(main_module);

    M_initOpcodes();
}

Engine::~Engine()
{
    if (m_module)
        delete m_module;

    for (auto& module : m_imports)
        module.setEngine(nullptr);

    m_main_module.setEngine(nullptr);
}

Object Engine::execute(Function const& fun, std::vector<Object> const& args)
{
    int tos = M_stackIndex();

    // Setup arguments
    m_argc = (int) args.size();
    for (int i = 0; i < m_argc; ++i)
        M_push(args[i]);

    // Push a dummy stack frame, when we will
    //   reach it execution will stop
    M_push(M_makeFrame(true));

    // Branch to the function
    M_branchToFunction(fun);

    while (M_execute());

    Object ret = Object::nil();
    if (M_stackIndex() != tos)
        ret = M_pop();

    return ret;
}

std::list<Module> const& Engine::imports() const
{ return m_imports; }

bool Engine::hasImported(std::string const& name) const
{
    for (auto const& module : m_imports)
        if (module.name() == name)
            return true;

    return false;
}

Module const& Engine::imported(std::string const& name) const
{
    for (auto const& module : m_imports)
        if (module.name() == name)
            return module;

    throw std::runtime_error("vm::Engine::imported: module `" + name + "' has never been imported");
}

void Engine::M_initOpcodes()
{
    #define OPCODE(name, nargs) m_opcodes_nargs[name] = nargs;
    #include "bits/opcodes.def"
    #undef OPCODE

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
        throw std::runtime_error("vm::Engine::M_top: stack is empty");
    return m_stack.back();
}

Object const& Engine::M_top() const
{
    if (!m_stack.size())
        throw std::runtime_error("vm::Engine::M_top: stack is empty");
    return m_stack.back();
}

Object Engine::M_pop()
{
    Object top = M_top();
    m_stack.pop_back();
    return top;
}

void Engine::M_push(Object const& value)
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
        throw std::runtime_error("vm::Engine::M_stackAt: invalid index");
    return m_stack.at(index);
}

core::Object const& Engine::M_stackAt(int index) const
{
    if (index < 0 || index >= (int) m_stack.size())
        throw std::runtime_error("vm::Engine::M_stackAt: invalid index");
    return m_stack.at(index);
}

void Engine::M_changeModule(Module const& module)
{
    if (m_module && module == *m_module)
        return;

    m_text = module.blob().text();
    if (!m_text)
        throw std::runtime_error("vm::Engine::M_changeModule: module has no text");

    if (m_module)
        delete m_module;
    m_module = new Module(module);
}

uint32_t Engine::M_fetch()
{
    if (!m_text)
        throw std::runtime_error("vm::Engine::M_fetch: no text");
    if (m_pc < 0 || m_pc >= (int) (m_text->size() / sizeof(uint32_t)))
        throw std::runtime_error("vm::Engine::M_fetch: invalid PC");

    return *((uint32_t*) m_text->raw(sizeof(uint32_t) * m_pc++, sizeof(uint32_t)));
}

void Engine::M_decode()
{
    // Get the opcode
    m_ir = (Opcode) M_fetch();
    if (!M_checkOpcode(m_ir))
        throw std::runtime_error("vm::Engine::M_fetch: invalid instruction opcode");

    // Get the operands
    m_operands.clear();
    for (int i = 0; i < m_opcodes_nargs[m_ir]; ++i)
        m_operands.push_back(M_fetch());
}

bool Engine::M_execute()
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
                M_error("M_execute: invalid local index");

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
                M_push(m_module->constant(index));
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
                M_error("M_execute: invalid string index");

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
                M_error("M_execute: invalid string index");

            // Get the object
            Object self = M_pop();

            if (m_ir == LOAD_MEMBER)
            {
                if (!self.has(name))
                    M_error("M_execute: member '" + name + "' does not exists for class '" + self.classname() + "'");
                M_push(self.member(name));
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
                M_error("M_execute: invalid operand");

            M_invoke(M_pop(), argc);
            break;
        }

        case METHOD:
        {
            std::string name;
            if (!m_module->blob().string(m_operands[0], name))
                M_error("M_execute: invalid string index");

            int argc = m_operands[1];
            if (argc < 0)
                M_error("M_execute: invalid operand");

            Object self = M_pop();
            if (!self.has(name))
                M_error("M_execute: method '" + name + "' does not exists for class '" + self.classname() + "'");

            // Insert the 'self' object on the stack
            std::vector<Object> argv(argc + 1, Object::nil());
            argv[0] = self;
            for (int i = 0; i < argc; ++i)
                argv[argc - i] = M_pop();

            Object const& fun = self.findPolymorphic(name, argv);

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
                M_error("M_execute: conditional is not a boolean");

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
                M_error("M_execute: conditional is not a boolean");

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
        case IMPORT_MASK:
        {
            std::string name;
            if (!m_module->blob().string(m_operands[0], name))
                M_error("M_execute: invalid string index");

            std::string mask = "";
            if (m_ir == IMPORT_MASK)
            {
                if (!m_module->blob().string(m_operands[1], mask))
                    M_error("M_execute: invalid string index");
            }

            if (!hasImported(name))
            {
                Module module(m_module->import(name, mask));
                module.setEngine(this);
                m_imports.push_back(module);
            }

            imported(name).global("__main__")();
            break;
        }

        default:
            M_error("M_execute: invalid opcode");
            break;
    }

    return true;
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

    while (fun.isInvokable() && !fun.isCallable())
        fun = fun.findPolymorphic(lang::std_call, argv);

    if (!fun.isCallable())
        M_error("M_invoke: object is not invokable");

    Callable call = fun.unwrap<Callable>();

    if (call.kind() == Callable::Kind::Native)
    {
        Object ret = call.invoke(argv);

        if (call.signature().returns())
            M_push(ret);
    }
    else
    {
        for (auto arg : argv)
            M_push(arg);

        M_push(M_makeFrame());
        M_branchToFunction(call.meta().as<Function>());
        m_argc = argc;
    }
}

bool Engine::M_leave()
{
    M_shrinkStack(m_locals_count);
    
    Object frame = M_pop();
    if (!frame.meta().is<StackFrame>())
        M_error("M_leave: can't find the call frame");

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
{ throw std::runtime_error("vm::Engine::" + msg + " (at " + opcode_as_string(m_ir) + ")"); }
