#include "lang/pass/add_implicit_return.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

AddImplicitReturn::~AddImplicitReturn()
{}

void AddImplicitReturn::visit(FunDeclNode* node)
{
    bool needs_fix = true;
    Node* to_fix = nullptr;

    if (node->siblings().size())
    {
        Node* last = node->siblings()[0]->last();
        if (!dynamic_cast<ReturnNode*>(last))
        {
            to_fix = last;
        }
        else
            needs_fix = false;

        node->siblings()[0]->accept(this);
    }

    if (needs_fix)
    {
        Token start_token;
        if (to_fix)
            start_token = to_fix->startToken();
        else
            start_token = node->startToken();

        ReturnNode* new_node = new ReturnNode(start_token);

        if (to_fix)
            to_fix->chain(new_node);
        else
            node->addSibling(new_node);
    }
    
    M_follow(node);
}
