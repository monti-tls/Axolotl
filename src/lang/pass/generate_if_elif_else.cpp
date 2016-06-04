#include "lang/pass/generate_if_elif_else.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_ir.hpp"

#include "lang/ast/ast.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateIfElifElse::~GenerateIfElifElse()
{
    emitLabel(m_last_node, m_end_label);
}

void GenerateIfElifElse::visit(IfNode* node)
{
    if (m_stop)
        return;
    
    m_end_label = newLabel();
    std::string block_end_label = newLabel();

    emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));
    emitGotoIfFalse(node, block_end_label);

    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[1], this));
    emitGoto(node, m_end_label);

    emitLabel(node, block_end_label);

    m_last_node = node;

    m_stop = true;
    M_follow(node);
}

void GenerateIfElifElse::visit(ElifNode* node)
{
    std::string block_end_label = newLabel();

    emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));
    emitGotoIfFalse(node, block_end_label);

    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[1], this));
    emitGoto(node, m_end_label);

    emitLabel(node, block_end_label);

    m_last_node = node;
    M_follow(node);
}

void GenerateIfElifElse::visit(ElseNode* node)
{
    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[0], this));

    m_last_node = node;
    M_follow(node);
}

void GenerateIfElifElse::visitDefault(Node*)
{ /* don't handle other blocks */ }
