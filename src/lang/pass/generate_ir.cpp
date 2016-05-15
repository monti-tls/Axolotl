#include "lang/pass/generate_ir.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_lvalue.hpp"

#include "lang/ast/ast.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateIR::~GenerateIR() {}

void GenerateIR::visit(ProgNode* node)
{
    Node* body = node->siblings()[0];

    IR_ProgNode* new_node = new IR_ProgNode(node->startToken());
    new_node->addSibling(NodeGenerator::generate<GenerateIR>(body, this));

    M_emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(FunDeclNode* node)
{
    Node* body = node->siblings()[0];

    IR_FunDeclNode* new_node = new IR_FunDeclNode(node->startToken());
    new_node->name = node->name;
    new_node->addSibling(NodeGenerator::generate<GenerateIR>(body, this));

    M_emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(ReturnNode* node)
{
    if (node->siblings().size())
    {
        M_emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));

        IR_ReturnNode* new_node = new IR_ReturnNode(node->startToken());
        M_emit(new_node);
    }
    else
    {
        IR_LeaveNode* new_node = new IR_LeaveNode(node->startToken());
        M_emit(new_node);
    }

    M_follow(node);
}

void GenerateIR::visit(AssignNode* node)
{
    M_emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[1], this));
    M_emit(NodeGenerator::generate<GenerateLValue>(node->siblings()[0], this));

    M_follow(node);
}

void GenerateIR::visitDefault(Node* node)
{
    Node* gen = NodeGenerator::generate<GenerateRValue>(node, this, true);
    M_emit(gen);

    Node::Flags flags = gen->last()->flags();

    if ((flags & Node::IR_Call) || (flags & Node::IR_Load))
    {
        IR_PopNode* new_node = new IR_PopNode(node->startToken());
        M_emit(new_node);
    }

    M_follow(node);
}
