#include "lang/pass_generate_ir.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_generator.hpp"
#include "lang/pass_generate_rvalue.hpp"
#include "lang/pass_generate_lvalue.hpp"

#include "lang/ast_xltl_node_visitor_impl.hpp"
#include "lang/ast_xltl_node_generator_impl.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateIR::~GenerateIR() {}

void GenerateIR::visit(XltlProgNode* node)
{
    XltlNode* body = node->siblings()[0];

    XltlIR_ProgNode* new_node = new XltlIR_ProgNode(node->startToken());
    new_node->addSibling(XltlNodeGenerator::generate<GenerateIR>(body, this));

    M_emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(XltlFunDeclNode* node)
{
    XltlNode* body = node->siblings()[0];

    XltlIR_FunDeclNode* new_node = new XltlIR_FunDeclNode(node->startToken());
    new_node->name = node->name;
    new_node->addSibling(XltlNodeGenerator::generate<GenerateIR>(body, this));

    M_emit(new_node);
    M_follow(node);
}

void GenerateIR::visit(XltlReturnNode* node)
{
    if (node->siblings().size())
    {
        M_emit(XltlNodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));

        XltlIR_ReturnNode* new_node = new XltlIR_ReturnNode(node->startToken());
        M_emit(new_node);
    }
    else
    {
        XltlIR_LeaveNode* new_node = new XltlIR_LeaveNode(node->startToken());
        M_emit(new_node);
    }

    M_follow(node);
}

void GenerateIR::visit(XltlAssignNode* node)
{
    M_emit(XltlNodeGenerator::generate<GenerateRValue>(node->siblings()[1], this));
    M_emit(XltlNodeGenerator::generate<GenerateLValue>(node->siblings()[0], this));

    M_follow(node);
}

void GenerateIR::visitDefault(XltlNode* node)
{
    XltlNode* gen = XltlNodeGenerator::generate<GenerateRValue>(node, this, true);
    M_emit(gen);

    XltlNode::Flags flags = gen->last()->flags();

    if ((flags & XltlNode::IR_Call) || (flags & XltlNode::IR_Load))
    {
        XltlIR_PopNode* new_node = new XltlIR_PopNode(node->startToken());
        M_emit(new_node);
    }

    M_follow(node);
}
