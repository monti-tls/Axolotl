#include "lang/pass_resolve_names.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ResolveNames::~ResolveNames()
{}

void ResolveNames::visit(XltlNameNode* node)
{
    XltlSymtab::FindResult res;
    if (node->symtab()->find(node->value, &res))
    {
        switch (res.symbol->which())
        {
            case XltlSymbol::Auto:
            {
                switch (res.symbol->binding())
                {
                    case XltlSymbol::Global:
                    {
                        XltlGlobalRefNode* new_node = new XltlGlobalRefNode(node->startToken());
                        new_node->name = node->value;

                        node->exchangeWith(new_node);
                        delete node;
                        M_follow(new_node);
                        return;
                    }

                    case XltlSymbol::Local:
                    {
                        XltlLocalRefNode* new_node = new XltlLocalRefNode(node->startToken());
                        new_node->index = res.index - res.args_count;

                        node->exchangeWith(new_node);
                        delete node;
                        M_follow(new_node);
                        return;
                    }

                    default:
                        break;
                }

                break;
            }

            case XltlSymbol::Argument:
            {
                if (res.locality != 0UL)
                    break;

                XltlConstRefNode* new_node = new XltlConstRefNode(node->startToken());
                new_node->index = - (((int) res.index) + 1);

                node->exchangeWith(new_node);
                delete node;
                M_follow(new_node);
                return;
            }

            default:
                break;
        }
    }

    M_error(node, "use of unbound name");
}
