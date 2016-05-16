#include "lang/pass/generate_while.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_ir.hpp"

#include "lang/ast/ast.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateWhile::~GenerateWhile()
{ }

void GenerateWhile::visit(WhileNode* node)
{
    std::string cond_label = M_newLabel();
    std::string end_label = M_newLabel();

    M_emitLabel(node, cond_label);
    M_emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));
    M_emitGotoIfFalse(node, end_label);

    M_emit(NodeGenerator::generate<GenerateIR>(node->siblings()[1], this));
    M_emitGoto(node, cond_label);

    M_emitLabel(node, end_label);
}

void GenerateWhile::visitDefault(Node*)
{ /* don't handle other blocks */ }
