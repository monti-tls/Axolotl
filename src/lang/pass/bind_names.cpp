#include "lang/pass/bind_names.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

BindNames::~BindNames()
{}

void BindNames::visit(AssignNode* node)
{
    NameNode* name = dynamic_cast<NameNode*>(node->siblings()[0]);
    if (name && !node->symtab()->find(name->value))
    {
        Symbol symbol(Symbol::Auto, name->value);
        node->symtab()->add(symbol);
    }

    M_follow(node);
}
