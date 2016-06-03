#include "lang/pass/expand_compound_assignments.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ExpandCompoundAssignments::~ExpandCompoundAssignments()
{}

void ExpandCompoundAssignments::visit(AssignNode* node)
{
    if (node->operation != "")
    {
        MethodNode* new_node = new MethodNode(node->startToken());
        new_node->name = node->operation;
        node->operation.clear();

        Node* right = node->siblings()[1];
        right->remove();

        new_node->addSibling(node->siblings()[0]->clone());
        new_node->addSibling(right);

        node->addSibling(new_node);
    }
    
    M_follow(node);
}
