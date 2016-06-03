#include "lang/pass/generate_ir.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_lvalue.hpp"
#include "lang/pass/generate_if_elif_else.hpp"
#include "lang/pass/generate_while.hpp"
#include "lang/pass/generate_for.hpp"

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
    new_node->attachSymtab(node->detachSymtab());

    emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(FunDeclNode* node)
{
    Node* body = node->siblings()[0];

    IR_FunDeclNode* new_node = new IR_FunDeclNode(node->startToken());
    new_node->name = node->name;
    new_node->addSibling(NodeGenerator::generate<GenerateIR>(body, this));
    new_node->attachSymtab(node->detachSymtab());

    emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(ClassDeclNode* node)
{
    Node* body = node->siblings()[0];

    IR_ClassDeclNode* new_node = new IR_ClassDeclNode(node->startToken());
    new_node->name = node->name;
    new_node->addSibling(NodeGenerator::generate<GenerateIR>(body, this));
    new_node->attachSymtab(node->detachSymtab());

    emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(ReturnNode* node)
{
    if (node->siblings().size())
    {
        emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));

        IR_ReturnNode* new_node = new IR_ReturnNode(node->startToken());
        emit(new_node);
    }
    else
    {
        IR_LeaveNode* new_node = new IR_LeaveNode(node->startToken());
        emit(new_node);
    }

    M_follow(node);
}

void GenerateIR::visit(AssignNode* node)
{
    emit(NodeGenerator::generate<GenerateRValue>(node->siblings()[1], this));
    emit(NodeGenerator::generate<GenerateLValue>(node->siblings()[0], this));

    M_follow(node);
}

void GenerateIR::visit(IfNode* node)
{
    emit(NodeGenerator::generate<GenerateIfElifElse>(node, this));
    M_follow(node);
}

void GenerateIR::visit(ElifNode* node)
{ /* already generated */ M_follow(node); }

void GenerateIR::visit(ElseNode* node)
{ /* already generated */ M_follow(node); }

void GenerateIR::visit(WhileNode* node)
{
    emit(NodeGenerator::generate<GenerateWhile>(node, this));
    M_follow(node);
}

void GenerateIR::visit(ForNode* node)
{
    emit(NodeGenerator::generate<GenerateFor>(node, this));
    M_follow(node);
}

void GenerateIR::visit(BreakNode* node)
{
    // Let one of our parents handle the node
    //   (this should be GenerateWhile or GenerateFor)
    hook(this, node);
    M_follow(node);
}

void GenerateIR::visit(ContinueNode* node)
{
    // Let one of our parents handle the node
    //   (this should be GenerateWhile or GenerateFor)
    hook(this, node);
    M_follow(node);
}

void GenerateIR::visit(ImportNode* node)
{
    IR_ImportNode* new_node = new IR_ImportNode(node->startToken());
    new_node->name = node->name;

    emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(ImportMaskNode* node)
{
    IR_ImportMaskNode* new_node = new IR_ImportMaskNode(node->startToken());
    new_node->name = node->name;
    new_node->mask = node->mask;

    emit(new_node);
    M_follow(node);
}

void GenerateIR::visitDefault(Node* node)
{
    Node* gen = NodeGenerator::generate<GenerateRValue>(node, this, true);
    emit(gen);

    Node::Flags flags = gen->last()->flags();

    if ((flags & Node::IR_Call) || (flags & Node::IR_Load))
    {
        IR_PopNode* new_node = new IR_PopNode(node->startToken());
        emit(new_node);
    }

    M_follow(node);
}
