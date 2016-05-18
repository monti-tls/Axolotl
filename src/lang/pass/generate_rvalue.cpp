#include "lang/pass/generate_rvalue.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

#include "lang/ast/ast.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateRValue::~GenerateRValue() {}

void GenerateRValue::visit(GlobalRefNode* node)
{
    IR_LoadGlobalNode* new_node = new IR_LoadGlobalNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(LocalRefNode* node)
{
    IR_LoadLocalNode* new_node = new IR_LoadLocalNode(node->startToken());
    new_node->index = node->index;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(ConstRefNode* node)
{
    IR_LoadConstNode* new_node = new IR_LoadConstNode(node->startToken());
    new_node->index = node->index;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(InvokeNode* node)
{
    int argc = 0;
    if (node->siblings().size() > 1)
    {
        // Avoid the nofollow flag for argument lists
        M_emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[1], this));
        argc = (int) node->siblings()[1]->chainLength();
    }

    node->siblings()[0]->accept(this);

    IR_InvokeNode* new_node = new IR_InvokeNode(node->startToken());
    new_node->argc = argc;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(MemberNode* node)
{
    node->siblings()[0]->accept(this);

    IR_LoadMemberNode* new_node = new IR_LoadMemberNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(MethodNode* node)
{
    int argc = 0;
    if (node->siblings().size() > 1)
    {
        // Avoid the nofollow flag for argument lists
        M_emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[1], this));
        argc = (int) node->siblings()[1]->chainLength();
    }

    node->siblings()[0]->accept(this);

    IR_MethodNode* new_node = new IR_MethodNode(node->startToken());
    new_node->name = node->name;
    new_node->argc = argc;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visitDefault(Node* node)
{ M_error(node, "not an rvalue"); }
