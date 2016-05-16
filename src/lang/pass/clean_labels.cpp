#include "lang/pass/clean_labels.hpp"
#include "lang/pass/rename_label.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

#include "lang/ast/ast.hpp"

#include <string>

using namespace lang;
using namespace ast;
using namespace pass;

CleanLabels::~CleanLabels()
{}

void CleanLabels::visit(IR_LabelNode* node)
{
    IR_LabelNode* next = nullptr;

    if (node->next() && (next = dynamic_cast<IR_LabelNode*>(node->next())))
    {
        std::string old_name = next->name;
        next->remove();
        delete next;

        NodeVisitor::apply<RenameLabel>(node->root(), this, old_name, node->name);
    }

    M_follow(node);
}
