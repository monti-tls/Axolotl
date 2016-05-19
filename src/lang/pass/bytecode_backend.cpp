#include "lang/pass/bytecode_backend.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"
#include "lang/parser_base.hpp"
#include "core/class.hpp"

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
    m_assembler->setDebugInfo(parser->streamName());
}

ByteCodeBackend::~ByteCodeBackend()
{ delete m_assembler; }

void ByteCodeBackend::finalize()
{ m_assembler->finalize(); }

Blob const& ByteCodeBackend::blob() const
{ return m_blob; }

void ByteCodeBackend::visit(IR_ProgNode* node)
{
    Symtab* top = node->symtab()->top();
    for (auto it = top->begin(); it != top->end(); ++it)
    {
        if (it->which() == Symbol::Const)
        {
            if (!m_blob.addConstant(it->data().classid(), it->data().serialize()))
                M_error(node, "internal error: unable to add constant entry to blob");
        }
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
            Class::Id id = it->data().unwrap<Class::Id>();
            if (!m_blob.addSignatureArgument(sigidx, id))
                M_error(node, "internal error: unable to insert signature argument");
        }
    }

    // Add symbol entry
    blob_idx symidx;
    blob_symbol* symbol = m_blob.addSymbol(node->name, &symidx);
    if (!symbol)
        M_error(node, "internal error: unable to insert symbol in blob");

    // Setup symbol entry
    symbol->s_addr = (blob_off) addr;
    symbol->s_nlocals = node->symtab()->localsCount();
    symbol->s_signature = sigidx;

    if (M_inClassDecl())
    {
        symbol->s_type = BLOB_SYMT_METHOD;
        symbol->s_bind = BLOB_SYMB_LOCAL;

        if (!m_blob.addTypeSpecSymbol(M_currentClassDeclIndex(), symidx))
            M_error(node, "internal error: unable to insert symbol to type spec in blob");
    }
    else
    {
        symbol->s_type = BLOB_SYMT_FUNCTION;
        symbol->s_bind = BLOB_SYMB_GLOBAL;
    }

    M_follow(node);
}

void ByteCodeBackend::visit(IR_ClassDeclNode* node)
{
    blob_idx tsidx;
    if (!m_blob.addTypeSpec(node->name, &tsidx))
        M_error(node, "internal error: unable to create a new type in blob");

    M_pushClassDeclIndex(tsidx);

    node->siblings()[0]->accept(this);

    M_popClassDeclIndex();

    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadConstNode* node)
{
    m_assembler->emit(LOAD_CONST, { node->index }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadGlobalNode* node)
{
    m_assembler->emit(LOAD_GLOBAL, { node->name }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_StorGlobalNode* node)
{
    m_assembler->emit(STOR_GLOBAL, { node->name }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadLocalNode* node)
{
    m_assembler->emit(LOAD_LOCAL, { node->index }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_StorLocalNode* node)
{
    m_assembler->emit(STOR_LOCAL, { node->index }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LoadMemberNode* node)
{
    m_assembler->emit(LOAD_MEMBER, { node->name }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_StorMemberNode* node)
{
    m_assembler->emit(STOR_MEMBER, { node->name }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LabelNode* node)
{
    m_assembler->label(node->name);
    M_follow(node);
}

void ByteCodeBackend::visit(IR_GotoNode* node)
{
    m_assembler->emit(JMP, { Operand::label(node->name) }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_GotoIfTrueNode* node)
{
    m_assembler->emit(JMP_IF_TRUE, { Operand::label(node->name) }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_GotoIfFalseNode* node)
{
    m_assembler->emit(JMP_IF_FALSE, { Operand::label(node->name) }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_InvokeNode* node)
{
    m_assembler->emit(INVOKE, { node->argc }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_MethodNode* node)
{
    m_assembler->emit(METHOD, { node->name, node->argc }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_ReturnNode* node)
{
    m_assembler->emit(RETURN, { }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_LeaveNode* node)
{
    m_assembler->emit(LEAVE, { }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_PopNode* node)
{
    m_assembler->emit(POP, { }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_ImportNode* node)
{
    m_assembler->emit(IMPORT, { node->name }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visit(IR_ImportMaskNode* node)
{
    m_assembler->emit(IMPORT_MASK, { node->name, node->mask }, node->startToken());
    M_follow(node);
}

void ByteCodeBackend::visitDefault(Node* node)
{ M_error(node, "unimplemented"); }

bool ByteCodeBackend::M_inClassDecl() const
{ return m_class_decls.size(); }

blob_idx ByteCodeBackend::M_currentClassDeclIndex() const
{ return m_class_decls.top(); }

void ByteCodeBackend::M_pushClassDeclIndex(blob_idx idx)
{ m_class_decls.push(idx); }

blob_idx ByteCodeBackend::M_popClassDeclIndex()
{
    blob_idx idx = M_currentClassDeclIndex();
    m_class_decls.pop();
    return idx;
}
