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

#include "bits/disassembler.hpp"
#include "bits/blob.hpp"
#include "bits/opcodes.hpp"

#include <iomanip>
#include <vector>
#include <map>
#include <sstream>

using namespace bits;

Disassembler::Disassembler(Blob const& blob, std::ostream& os)
    : m_blob(blob)
    , m_os(os)
{}

void Disassembler::dumpAll()
{
    dumpStrings();
    m_os << std::endl;
    dumpSymbols();
    m_os << std::endl;
    dumpTypeSpecs();
    m_os << std::endl;
    dumpConstants();
    m_os << std::endl;
    dumpText();
}

void Disassembler::dumpStrings()
{
    m_os << "Strings :" << std::endl;
    m_blob.foreachString([&](blob_idx sidx, std::string const& str)
    {
        m_os << "  [" << std::setw(4) << std::hex << sidx << std::dec << "] ";
        m_os << str << std::endl;
    });
}

void Disassembler::dumpSymbols()
{
    m_os << "Symbols :" << std::endl;
    m_blob.foreachSymbol([&](blob_idx symidx, blob_symbol* sym)
    {
        m_os << "  [" << std::setw(4) << symidx << "] ";

        if (sym->s_bind == BLOB_SYMB_NULL)
            m_os << "NULL   ";
        else if (sym->s_bind == BLOB_SYMB_LOCAL)
            m_os << "LOCAL  ";
        else if (sym->s_bind == BLOB_SYMB_GLOBAL)
            m_os << "GLOBAL ";

        if (sym->s_type == BLOB_SYMT_NULL)
            m_os << "NULL     ";
        else if (sym->s_type == BLOB_SYMT_FUNCTION)
            m_os << "FUNCTION ";
        else if (sym->s_type == BLOB_SYMT_METHOD)
            m_os << "METHOD   ";

        m_os << std::setw(8) << std::setfill('0') << std::hex
           << sym->s_addr << std::setfill(' ') << std::dec << " ";

        m_os << "locals:" << std::setw(2) << sym->s_nlocals << " ";

        m_os << m_blob.string(sym->s_name);
        M_dumpSignature(sym->s_signature);

        m_os << std::endl;
    });
}

void Disassembler::dumpTypeSpecs()
{
    m_os << "Type specifications :" << std::endl;
    m_blob.foreachTypeSpec([&](blob_idx tsidx, blob_typespec* tspec)
    {
        m_os << "  [" << std::setw(4) << tsidx << "] " << m_blob.string(tspec->ts_name) << std::endl;
        m_blob.foreachTypeSpecSymbol(tsidx, [&](blob_idx symidx)
        {
            blob_symbol* sym = m_blob.symbol(symidx);
            m_os << "         [" << std::setw(4) << symidx << "] " << m_blob.string(sym->s_name);
            M_dumpSignature(sym->s_signature);
            m_os << std::endl;
        });
    });
}

void Disassembler::dumpConstants()
{
    m_os << "Constants :" << std::endl;
    m_blob.foreachConstant([&](blob_idx cstidx, blob_constant* cst)
    {
        m_os << "  [" << std::setw(4) << cstidx << "] ";
        m_os << std::setw(8) << std::left << m_blob.string(cst->c_type) << std::right << " ";
        m_os << '\'' << m_blob.string(cst->c_serialized) << '\'' << std::endl;
    });
}

void Disassembler::dumpText()
{
    m_os << "Text section :" << std::endl;

    std::shared_ptr<Buffer> text = m_blob.text();
    if (!text)
        return;
    int count = (int) (text->size() / sizeof(uint32_t));

    auto decodeInstruction = [&](int& pc, std::vector<int>& operands)
    {
        static std::map<Opcode, int> opcodes_nargs;
        static bool opcodes_nargs_inited = false;

        if (!opcodes_nargs_inited)
        {   
            #define OPCODE(name, nargs) opcodes_nargs[name] = nargs;
            #include "bits/opcodes.inc"
            #undef OPCODE

            opcodes_nargs_inited = true;
        }

        auto fetch = [&]()
        { return *((uint32_t*) text->raw(sizeof(uint32_t) * pc++, sizeof(uint32_t))); };

        Opcode opcode = (Opcode) fetch();
        for (int i = 0; i < opcodes_nargs[opcode]; ++i)
            operands.push_back((int) fetch());

        return opcode;
    };

    // Find all jump targets and allocate label names to them
    int jmp_targets_count = 0;
    std::map<int, std::pair<std::string, std::vector<int>>> jmp_targets;
    for (int pc = 0; pc < count; )
    {
        std::vector<int> operands;
        Opcode opcode = decodeInstruction(pc, operands);

        bool is_jmp = false;
        int target = 0;
        switch (opcode)
        {
            case JMPR:
            case JMPR_IF_FALSE:
            case JMPR_IF_TRUE:
            {
                is_jmp = true;
                target = pc + operands[0];
                break;
            }

            case JMP:
            case JMP_IF_FALSE:
            case JMP_IF_TRUE:
            {
                is_jmp = true;
                target = operands[0];
                break;
            }

            default:
                break;
        }

        if (is_jmp)
        {
            if (jmp_targets.find(target) == jmp_targets.end())
            {
                std::ostringstream ss;
                ss << "t" << std::setw(3) << std::setfill('0') << jmp_targets_count++;
                jmp_targets[target].first = ss.str();
            }
            jmp_targets[target].second.push_back(pc);
        }
    }

    // Display the actual disassembly
    for (int pc = 0; pc < count; )
    {
        // Find if this instruction begins a symbol
        blob_symbol* symbol = nullptr;
        m_blob.foreachSymbol([&](blob_idx, blob_symbol* sym)
        {
            if ((int) sym->s_addr == pc)
                symbol = sym;
        });

        // If yes, display the symbol prototype
        if (symbol)
        {
            m_os << std::endl << m_blob.string(symbol->s_name);
            M_dumpSignature(symbol->s_signature);
            m_os << ':' << std::endl;
        }

        // If this instruction is a jump target, display its label
        bool is_jmp_target = jmp_targets.find(pc) != jmp_targets.end();
        if (is_jmp_target)
        {
            m_os << jmp_targets[pc].first << " > ";
        }
        else
            m_os << "       ";

        // Decode the isntruction
        std::vector<int> operands;
        Opcode opcode = decodeInstruction(pc, operands);

        // Display the mnemonic
        m_os << std::setw(20) << std::left << opcode_as_string(opcode) << std::right;

        switch (opcode)
        {
            case LOAD_CONST:
            {
                if (operands[0] < 0)
                {
                    m_os << "a" << (-1-operands[0]);
                }
                else
                {
                    blob_constant* cst = m_blob.constant(operands[0]);
                    if (!cst)
                        m_os << "<invalid>";
                    else
                        m_os << "(" << m_blob.string(cst->c_type) << ") '" << m_blob.string(cst->c_serialized) << "'";
                }
                break;
            }

            case LOAD_LOCAL:
            case STOR_LOCAL:
            {
                m_os << "l" << operands[0];
                break;
            }

            case LOAD_GLOBAL:
            case STOR_GLOBAL:
            {
                std::string name;
                if (!m_blob.string(operands[0], name))
                    name = "<invalid>";
                m_os << name;
                break;
            }

            case INVOKE:
                m_os << operands[0];
                break;

            case METHOD:
            {
                std::string name;
                if (!m_blob.string(operands[0], name))
                    name = "<invalid>";
                m_os << name << ", " << operands[1];
                break;
            }

            case JMPR:
            case JMPR_IF_FALSE:
            case JMPR_IF_TRUE:
                m_os << jmp_targets[pc + operands[0]].first;
                break;

            case JMP:
            case JMP_IF_FALSE:
            case JMP_IF_TRUE:
                m_os << jmp_targets[operands[0]].first;
                break;

            default:
                break;
        }

        m_os << std::endl;
    }
}

void Disassembler::M_dumpSignature(blob_idx sigidx)
{
    std::vector<std::string> args;
    m_blob.foreachSignatureArgument(sigidx, [&](blob_off soff)
    { args.push_back(m_blob.string(soff)); });

    m_os << '(';
    for (int i = 0; i < (int) args.size(); ++i)
        m_os << args[i] << (i == (int) args.size() - 1 ? "" : ", ");
    m_os << ')';
}
