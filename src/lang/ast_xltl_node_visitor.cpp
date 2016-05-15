#include "lang/ast_xltl_node_visitor.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/parser.hpp"

using namespace lang;
using namespace ast;

AbstractXltlNodeVisitor::AbstractXltlNodeVisitor(Parser* parser, bool nofollow)
    : m_parser(parser)
    , m_nofollow(nofollow)
{}

AbstractXltlNodeVisitor::AbstractXltlNodeVisitor(XltlNodeVisitor* parent, bool nofollow)
    : m_parser(parent->m_parser)
    , m_nofollow(nofollow)
{}

AbstractXltlNodeVisitor::~AbstractXltlNodeVisitor()
{}

void AbstractXltlNodeVisitor::init()
{}

void AbstractXltlNodeVisitor::M_message(XltlNode* node, std::string const& msg)
{ m_parser->message(node->startToken(), msg); }

void AbstractXltlNodeVisitor::M_error(XltlNode* node, std::string const& msg)
{ m_parser->error(node->startToken(), msg); }

void AbstractXltlNodeVisitor::M_warning(XltlNode* node, std::string const& msg)
{ m_parser->warning(node->startToken(), msg); }

void AbstractXltlNodeVisitor::M_follow(XltlNode* node)
{
    if (!m_nofollow && node && node->next())
        node->next()->accept(this);
}

XltlNodeVisitor::~XltlNodeVisitor()
{}

void XltlNodeVisitor::init()
{ AbstractXltlNodeVisitor::init(); }

void XltlNodeVisitor::visitDefault(XltlNode* node)
{
    for (auto sib : node->siblings())
            sib->accept(this);
    M_follow(node);
}

#define CAT(a, b, c) a ## b ## c
#define DEF_FLAG(name, value)
#define DEF_NODE(name, ...) \
    void XltlNodeVisitor::visit(CAT(Xltl, name, Node)* node) \
    { visitDefault(node); }
#include "lang/ast_xltl_nodes.def"
#undef DEF_NODE
#undef DEF_FLAG
#undef CAT
