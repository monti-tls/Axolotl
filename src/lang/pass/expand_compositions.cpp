#include "lang/pass/expand_compositions.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ExpandCompositions::~ExpandCompositions()
{}

void ExpandCompositions::visit(ComposeNode* node)
{
    node->siblings()[0]->accept(this);
    node->siblings()[1]->accept(this);

    Node* a = node->siblings()[0];
    Node* b = node->siblings()[1];

    a->remove();
    b->remove();
    b->chain(a);

    NameNode* name_node = new NameNode(node->startToken());
    //!FIXME: WTF?!
    name_node->value = "@core.compose";

    InvokeNode* invoke_node = new InvokeNode(node->startToken());
    invoke_node->addSibling(name_node);
    invoke_node->addSibling(b);

    node->replaceBy(invoke_node);
    delete node;
    
    M_follow(invoke_node);
}
