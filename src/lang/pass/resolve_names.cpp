#include "lang/pass/resolve_names.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ResolveNames::~ResolveNames()
{}

void ResolveNames::visit(NameNode* node)
{
    Symtab::FindResult res;
    if (node->symtab()->find(node->value, &res))
    {
        switch (res.symbol->which())
        {
            case Symbol::Auto:
            {
                switch (res.symbol->binding())
                {
                    case Symbol::Global:
                    {
                        GlobalRefNode* new_node = new GlobalRefNode(node->startToken());
                        new_node->name = node->value;

                        node->exchangeWith(new_node);
                        delete node;
                        M_follow(new_node);
                        return;
                    }

                    case Symbol::Local:
                    {
                        LocalRefNode* new_node = new LocalRefNode(node->startToken());
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

            case Symbol::Argument:
            {
                if (res.locality != 0UL)
                    break;

                ConstRefNode* new_node = new ConstRefNode(node->startToken());
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
