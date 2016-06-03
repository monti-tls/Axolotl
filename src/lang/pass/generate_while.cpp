#include "lang/pass/generate_while.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_ir.hpp"

#include "lang/ast/ast.hpp"

#include <iostream>

using namespace lang;
using namespace ast;
using namespace pass;

GenerateWhile::~GenerateWhile()
{ }

void GenerateWhile::visit(WhileNode* node)
{
    m_cond_label = newLabel();
    m_end_label = newLabel();

    emitLabel(node, m_cond_label);
    emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));
    emitGotoIfFalse(node, m_end_label);

    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[1], this));
    emitGoto(node, m_cond_label);

    emitLabel(node, m_end_label);
}

void GenerateWhile::visitDefault(Node*)
{ /* don't handle other blocks */ }

void GenerateWhile::hook(NodeGenerator* gen, BreakNode* node)
{
    gen->emitGoto(node, m_end_label);
}

void GenerateWhile::hook(NodeGenerator* gen, ContinueNode* node)
{
    gen->emitGoto(node, m_cond_label);
}
