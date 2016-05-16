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

#ifndef __AXOLOTL_VM_ENGINE_H__
#define __AXOLOTL_VM_ENGINE_H__

#include "vm/module.hpp"
#include "vm/stack_frame.hpp"
#include "core/object.hpp"

#include <string>
#include <map>
#include <vector>
#include <list>

namespace vm
{
    class Engine
    {
    public:
        Engine(Module const& main_module);
        ~Engine();

        core::Object execute(Function const& fun, std::vector<core::Object> const& args);

        std::list<Module> const& imports() const;
        bool hasImported(std::string const& name) const;
        Module const& imported(std::string const& name) const;

    private:
        void M_initOpcodes();
        bool M_checkOpcode(bits::Opcode opcode) const;

        core::Object& M_top();
        core::Object const& M_top() const;
        core::Object M_pop();
        void M_push(core::Object const& value);
        void M_growStack(std::size_t amount);
        void M_shrinkStack(std::size_t amount);
        int M_stackIndex() const;
        core::Object& M_stackAt(int index);
        core::Object const& M_stackAt(int index) const;

        void M_changeModule(Module const& module);
        uint32_t M_fetch();
        void M_decode();
        bool M_execute();
        StackFrame M_makeFrame(bool dummy = false) const;
        bool M_setFrame(StackFrame const& frame);
        void M_invoke(core::Object fun, int argc);
        bool M_leave();
        void M_branchToFunction(Function const& fun);
        void M_error(std::string const& msg) const;

    private:
        Module m_main_module;
        std::list<Module> m_imports;

        std::vector<core::Object> m_stack;
        std::shared_ptr<bits::Buffer> m_text;

        bits::Opcode m_ir;
        std::vector<uint32_t> m_operands;

        int m_pc;
        Module* m_module;
        int m_locals_start;
        int m_locals_count;
        int m_argc;

        std::map<bits::Opcode, int> m_opcodes_nargs;
    };
}

#endif // __AXOLOTL_VM_ENGINE_H__
