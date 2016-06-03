#include "lang/ast/node_visitor.hpp"
#include "lang/ast/node.hpp"
#include "lang/parser.hpp"

using namespace lang;
using namespace ast;

AbstractNodeVisitor::AbstractNodeVisitor(ParserBase* parser, bool nofollow)
    : m_parser(parser)
    , m_parent(nullptr)
    , m_nofollow(nofollow)
{}

AbstractNodeVisitor::AbstractNodeVisitor(AbstractNodeVisitor* parent, bool nofollow)
    : m_parser(parent->m_parser)
    , m_parent(parent)
    , m_nofollow(nofollow)
{}

AbstractNodeVisitor::~AbstractNodeVisitor()
{}

void AbstractNodeVisitor::init()
{}

ParserBase* AbstractNodeVisitor::parser() const
{ return m_parser; }

AbstractNodeVisitor* AbstractNodeVisitor::parent() const
{ return m_parent; }

void AbstractNodeVisitor::M_message(Node* node, std::string const& msg)
{ m_parser->message(node->startToken(), msg); }

void AbstractNodeVisitor::M_error(Node* node, std::string const& msg)
{ m_parser->error(node->startToken(), msg); }

void AbstractNodeVisitor::M_warning(Node* node, std::string const& msg)
{ m_parser->warning(node->startToken(), msg); }

void AbstractNodeVisitor::M_follow(Node* node)
{
    if (!m_nofollow && node && node->next())
        node->next()->accept(this);
}

NodeVisitor::~NodeVisitor()
{}

void NodeVisitor::init()
{ AbstractNodeVisitor::init(); }

void NodeVisitor::visitDefault(Node* node)
{
    for (auto sib : node->siblings())
            sib->accept(this);
    M_follow(node);
}

#define CAT(a, b) a ## b
#define DEF_FLAG(name, value)
#define DEF_NODE(name, ...) \
    void NodeVisitor::visit(CAT(name, Node)* node) \
    { visitDefault(node); }
#include "lang/ast/nodes.def"
#undef DEF_NODE
#undef DEF_FLAG
#undef CAT
