#include "lang/pass/generate_for.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_ir.hpp"

#include "lang/ast/ast.hpp"

#include <iostream>

using namespace lang;
using namespace ast;
using namespace pass;

GenerateFor::~GenerateFor()
{ }

void GenerateFor::visit(ForNode* node)
{
    m_cond_label = newLabel();
    m_it_label = newLabel();
    m_end_label = newLabel();

    // Initialization
    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[0], this));

    // Condition evaluation
    emitLabel(node, m_cond_label);
    emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[1], this));
    emitGotoIfFalse(node, m_end_label);

    // Body
    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[3], this));

    // Iteration
    emitLabel(node, m_it_label);
    emit(NodeGenerator::generate<GenerateIR>(node->siblings()[2], this));
    emitGoto(node, m_cond_label);

    emitLabel(node, m_end_label);
}

void GenerateFor::visitDefault(Node*)
{ /* don't handle other blocks */ }

void GenerateFor::hook(NodeGenerator* gen, BreakNode* node)
{
    gen->emitGoto(node, m_end_label);
}

void GenerateFor::hook(NodeGenerator* gen, ContinueNode* node)
{
    gen->emitGoto(node, m_it_label);
}
