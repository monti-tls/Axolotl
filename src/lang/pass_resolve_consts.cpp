#include "lang/pass_resolve_consts.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"
#include "lib/lib.hpp"

using namespace lang;
using namespace ast;
using namespace pass;
using namespace lib;

ResolveConsts::~ResolveConsts()
{}

void ResolveConsts::init()
{ m_alloc_index = 0; }

void ResolveConsts::visit(XltlConstNode* node)
{
    XltlSymtab::FindResult res;
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

    XltlConstRefNode* new_node = new XltlConstRefNode(node->startToken());
    new_node->index = index;

    node->exchangeWith(new_node);
    delete node;

    M_follow(new_node);
}
