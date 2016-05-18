#include "lang/pass/resolve_consts.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"
#include "lib/lib.hpp"

using namespace lang;
using namespace ast;
using namespace pass;
using namespace lib;

ResolveConsts::~ResolveConsts()
{}

void ResolveConsts::init()
{ m_alloc_index = 0; }

void ResolveConsts::visit(ConstNode* node)
{
    Symtab* top = node->symtab()->top();

    bool found = false;
    int index = 0;
    for (auto it = top->begin(); it != top->end(); ++it)
    {
        if (it->which() == Symbol::Const)
        {
            if (it->data().classid() == node->value.classid() &&
                it->data().serialize() == node->value.serialize())
            {
                found = true;
                break;
            }

            ++index;
        }
    }

    if (!found)
    {
        index = m_alloc_index++;
        Symbol sym(Symbol::Const, Symbol::Global, "", node->value);
        top->add(sym);
    }

    ConstRefNode* new_node = new ConstRefNode(node->startToken());
    new_node->index = index;

    node->exchangeWith(new_node);
    delete node;

    M_follow(new_node);
}
