#include "lang/pass_generate_rvalue.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateRValue::~GenerateRValue() {}

void GenerateRValue::visit(XltlGlobalRefNode* node)
{
    XltlIR_LoadGlobalNode* new_node = new XltlIR_LoadGlobalNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(XltlLocalRefNode* node)
{
    XltlIR_LoadLocalNode* new_node = new XltlIR_LoadLocalNode(node->startToken());
    new_node->index = node->index;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(XltlConstRefNode* node)
{
    XltlIR_LoadConstNode* new_node = new XltlIR_LoadConstNode(node->startToken());
    new_node->index = node->index;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(XltlInvokeNode* node)
{
    int argc = 0;
    if (node->siblings().size() > 1)
    {
        node->siblings()[1]->accept(this);
        argc = (int) node->siblings()[1]->chainLength();
    }

    node->siblings()[0]->accept(this);

    XltlIR_InvokeNode* new_node = new XltlIR_InvokeNode(node->startToken());
    new_node->argc = argc;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(XltlMemberNode* node)
{
    node->siblings()[0]->accept(this);

    XltlIR_LoadMemberNode* new_node = new XltlIR_LoadMemberNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visit(XltlMethodNode* node)
{
    int argc = 0;
    if (node->siblings().size() > 1)
    {
        node->siblings()[1]->accept(this);
        argc = (int) node->siblings()[1]->chainLength();
    }

    node->siblings()[0]->accept(this);

    XltlIR_MethodNode* new_node = new XltlIR_MethodNode(node->startToken());
    new_node->name = node->name;
    new_node->argc = argc;

    M_emit(new_node);
    M_follow(node);
}

void GenerateRValue::visitDefault(XltlNode* node)
{ M_error(node, "not an rvalue"); }
