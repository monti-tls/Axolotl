#include "lang/pass_expr_result_check.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ExprResultCheck::~ExprResultCheck()
{}

void ExprResultCheck::visit(XltlMemberNode* node)
{
    M_warning(node, "expression result unused");
    M_follow(node);
}

void ExprResultCheck::visit(XltlNameNode* node)
{
    M_warning(node, "expression result unused");
    M_follow(node);
}

void ExprResultCheck::visitDefault(XltlNode* node)
{
    XltlNode::Flags flags = node->flags();
    if (flags & XltlNode::Block)
    {
        if (flags & XltlNode::HasCond)
            node->siblings()[1]->accept(this);
        else
            node->siblings()[0]->accept(this);
    }
    M_follow(node);
}
