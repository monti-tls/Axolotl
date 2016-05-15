#include "lang/pass/generate_lvalue.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"
#include "lang/ast/node_generator.hpp"
#include "lang/pass/generate_rvalue.hpp"

#include "lang/ast/ast.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateLValue::~GenerateLValue() {}

void GenerateLValue::visit(GlobalRefNode* node)
{
    IR_StorGlobalNode* new_node = new IR_StorGlobalNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
}

void GenerateLValue::visit(LocalRefNode* node)
{
    IR_StorLocalNode* new_node = new IR_StorLocalNode(node->startToken());
    new_node->index = node->index;

    M_emit(new_node);
}

void GenerateLValue::visit(MemberNode* node)
{
    M_emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));

    IR_StorMemberNode* new_node = new IR_StorMemberNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
}

void GenerateLValue::visitDefault(Node* node)
{ M_error(node, "not an lvalue"); }
