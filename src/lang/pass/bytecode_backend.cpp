#include "lang/pass/bytecode_backend.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

#include "lang/std_names.hpp"
#include "lib/dict.hpp"

#include <vector>
#include <algorithm>

using namespace lang;
using namespace ast;
using namespace pass;
using namespace bits;
using namespace lib;
using namespace core;

ByteCodeBackend::ByteCodeBackend(ParserBase* parser)
    : NodeVisitor(parser)
    , m_assembler(nullptr)
{
    m_assembler = new Assembler(m_blob);
}

ByteCodeBackend::~ByteCodeBackend()
{ delete m_assembler; }

void ByteCodeBackend::finalize()
{ m_assembler->finalize(); }

Blob const& ByteCodeBackend::blob() const
{ return m_blob; }

void ByteCodeBackend::visit(IR_ProgNode* node)
{
    // Get the const dict
    Symtab::FindResult res;
    node->symtab()->find(std_const_dict, &res);
    Dict const& const_dict = res.symbol->data().unwrap<Dict>();

    // Turn it into a vector
    std::vector<std::pair<int, Object>> const_array;
    std::transform(const_dict.begin(), const_dict.end(), std::back_inserter(const_array),
                   [](std::pair<Object, Object> it)
                   { return std::make_pair(it.second.unwrap<int>(), it.first); });

    // Sort the vector according to the indices
    std::sort(const_array.begin(), const_array.end(),
              [](std::pair<int, Object> const& a, std::pair<int, Object> const& b)
              { return a.first < b.first; });

    // Finally insert all constants in the blob
    for (auto it = const_array.begin(); it != const_array.end(); ++it)
    {
        if (!m_blob.addConstant(it->second.classname(), it->second.serialize()))
            M_error(node, "internal error: unable to add constant entry to blob");
    }

    // Generate bytecode for the whole program
    node->siblings()[0]->accept(this);
}

void ByteCodeBackend::visit(IR_FunDeclNode* node)
{
    // Record function address
    std::size_t addr = m_assembler->pos();

    // Generate bytecode for function
    node->siblings()[0]->accept(this);

    // Add function signature
    blob_idx sigidx;
    if (!m_blob.addSignature(sigidx))
        M_error(node, "internal error: unable to insert symbol signature in blob");

    // Populate function signature
    for (auto it = node->symtab()->begin(); it != node->symtab()->end(); ++it)
    {
        if (it->which() == Symbol::Argument)
        {
            std::string pattern = it->data().unwrap<std::string>();
            if (!m_blob.addSignatureArgument(sigidx, pattern))
                M_error(node, "internal error: unable to insert signature argument");
        }
    }

    // Add symbol entry
    blob_symbol* symbol = m_blob.addSymbol(node->name);
    if (!symbol)
        M_error(node, "internal error: unable to insert symbol in blob");

    // Setup symbol entry
    symbol->s_type = BLOB_SYMT_FUNCTION;
    symbol->s_bind = BLOB_SYMB_GLOBAL;
    symbol->s_addr = (blob_off) addr;
    symbol->s_nlocals = node->symtab()->localsCount();
    symbol->s_signature = sigidx;

    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadConstNode* node)
{
    m_assembler->emit(LOAD_CONST, { node->index });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadGlobalNode* node)
{
    m_assembler->emit(LOAD_GLOBAL, { node->name });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_StorGlobalNode* node)
{
    m_assembler->emit(STOR_GLOBAL, { node->name });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadLocalNode* node)
{
    m_assembler->emit(LOAD_LOCAL, { node->index });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_StorLocalNode* node)
{
    m_assembler->emit(STOR_LOCAL, { node->index });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadMemberNode* node)
{
    m_assembler->emit(LOAD_MEMBER, { node->name });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_StorMemberNode* node)
{
    m_assembler->emit(STOR_MEMBER, { node->name });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LabelNode* node)
{
    m_assembler->label(node->name);
    M_follow(node);
}

void ByteCodeBackend::visit(IR_GotoNode* node)
{
    m_assembler->emit(JMP, { Operand::label(node->name) });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_GotoIfTrueNode* node)
{
    m_assembler->emit(JMP_IF_TRUE, { Operand::label(node->name) });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_GotoIfFalseNode* node)
{
    m_assembler->emit(JMP_IF_FALSE, { Operand::label(node->name) });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_InvokeNode* node)
{
    m_assembler->emit(INVOKE, { node->argc });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_MethodNode* node)
{
    m_assembler->emit(METHOD, { node->name, node->argc });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_ReturnNode* node)
{
    m_assembler->emit(RETURN, { });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LeaveNode* node)
{
    m_assembler->emit(LEAVE, { });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_PopNode* node)
{
    m_assembler->emit(POP, { });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_ImportNode* node)
{
    m_assembler->emit(IMPORT, { node->name });
    M_follow(node);
}

void ByteCodeBackend::visit(IR_ImportMaskNode* node)
{
    m_assembler->emit(IMPORT_MASK, { node->name, node->mask });
    M_follow(node);
}

void ByteCodeBackend::visitDefault(Node* node)
{ M_error(node, "unimplemented"); }
