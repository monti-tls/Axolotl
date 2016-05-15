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
    Symtab::FindResult res;
    node->symtab()->find(std_const_dict, &res);

    Dict& const_dict = res.symbol->data().unwrap<Dict>();

    int index = 0;

    Dict::const_iterator it = const_dict.find(node->value);
    if (it == const_dict.end())
    {
        index = m_alloc_index++;
        const_dict.set(node->value, index);
    }
    else
        index = it->second.unwrap<int>();

    ConstRefNode* new_node = new ConstRefNode(node->startToken());
    new_node->index = index;

    node->exchangeWith(new_node);
    delete node;

    M_follow(new_node);
}
