#include "lang/pass_bind_names.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

BindNames::~BindNames()
{}

void BindNames::visit(XltlAssignNode* node)
{
    XltlNameNode* name = dynamic_cast<XltlNameNode*>(node->siblings()[0]);
    if (name && !node->symtab()->find(name->value))
    {
        XltlSymbol symbol(XltlSymbol::Auto, name->value);
        node->symtab()->add(symbol);
    }

    M_follow(node);
}
