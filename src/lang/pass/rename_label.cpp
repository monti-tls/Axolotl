#include "lang/pass/rename_label.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

RenameLabel::RenameLabel(ParserBase* parent, std::string const& from, std::string const& to, bool nofollow)
    : NodeVisitor(parent, nofollow)
    , m_from(from)
    , m_to(to)
{}

RenameLabel::RenameLabel(AbstractNodeVisitor* parent, std::string const& from, std::string const& to, bool nofollow)
    : NodeVisitor(parent, nofollow)
    , m_from(from)
    , m_to(to)
{}

RenameLabel::~RenameLabel()
{}

void RenameLabel::visit(IR_GotoNode* node)
{
    if (node->name == m_from)
        node->name = m_to;

    M_follow(node);
}

void RenameLabel::visit(IR_GotoIfTrueNode* node)
{
    if (node->name == m_from)
        node->name = m_to;

    M_follow(node);
}

void RenameLabel::visit(IR_GotoIfFalseNode* node)
{
    if (node->name == m_from)
        node->name = m_to;

    M_follow(node);
}
