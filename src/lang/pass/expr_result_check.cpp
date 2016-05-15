#include "lang/pass/expr_result_check.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ExprResultCheck::~ExprResultCheck()
{}

void ExprResultCheck::visit(MemberNode* node)
{
    M_warning(node, "expression result unused");
    M_follow(node);
}

void ExprResultCheck::visit(NameNode* node)
{
    M_warning(node, "expression result unused");
    M_follow(node);
}

void ExprResultCheck::visitDefault(Node* node)
{
    Node::Flags flags = node->flags();
    if (flags & Node::Block)
    {
        if (flags & Node::HasCond)
            node->siblings()[1]->accept(this);
        else
            node->siblings()[0]->accept(this);
    }
    M_follow(node);
}
