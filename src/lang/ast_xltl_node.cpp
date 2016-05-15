#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

using namespace lang;
using namespace ast;

XltlNode::XltlNode(Token const& start_token)
    : m_start_token(start_token)
    , m_prev(nullptr)
    , m_next(nullptr)
    , m_parent(nullptr)
    , m_symtab(nullptr)
{}

XltlNode::~XltlNode()
{
    if (m_symtab)
        delete m_symtab;

    if (m_next)
        delete m_next;
    
    for (auto sib : m_siblings)
        delete sib;
}

Token const& XltlNode::startToken() const
{ return m_start_token; }

std::vector<XltlNode*> const& XltlNode::siblings() const
{ return m_siblings; }

void XltlNode::addSibling(XltlNode* sibling)
{
    if (sibling)
    {
        sibling->setParent(this);
        m_siblings.push_back(sibling);
    }
}

void XltlNode::chain(XltlNode* node)
{
    node->setParent(m_parent);
    
    if (m_next)
        m_next->chain(node);
    else
    {
        m_next = node;
        node->m_prev = this;
    }
}

XltlNode* XltlNode::prev() const
{ return m_prev; }

XltlNode* XltlNode::next() const
{ return m_next; }

XltlNode* XltlNode::last() const
{
    if (!m_next)
        return const_cast<XltlNode*>(this);

    return m_next->last();
}

std::size_t XltlNode::chainLength() const
{
    std::size_t i = 0;
    for (XltlNode const* node = this; node; node = node->m_next)
        ++i;
    return i;
}

void XltlNode::setParent(XltlNode* parent)
{
    m_parent = parent;

    if (m_next)
        m_next->setParent(parent);
}

XltlNode* XltlNode::parent() const
{ return m_parent; }

void XltlNode::attachSymtab(XltlSymtab* symtab)
{
    if (m_symtab)
        delete m_symtab;

    m_symtab = symtab;
}

XltlSymtab* XltlNode::symtab() const
{
    if (m_symtab)
        return m_symtab;
    else if (m_parent)
        return m_parent->symtab();

    return nullptr;
}

void XltlNode::exchangeWith(XltlNode* node)
{
    node->m_parent = m_parent;
    node->m_symtab = m_symtab;

    if (m_parent)
    {
        for (std::vector<XltlNode*>::iterator it = m_parent->m_siblings.begin();
             it != m_parent->m_siblings.end(); ++it)
        {
            if (*it == this)
                *it = node;
        }
    }

    m_parent = nullptr;
    m_symtab = nullptr;

    XltlNode*& last = node->M_last();

    if (m_prev)
        m_prev->m_next = node;
    if (m_next)
        m_next->m_prev = last;

    node->m_prev = m_prev;
    last = m_next;

    m_next = nullptr;
    m_prev = nullptr;

    node->m_siblings = m_siblings;
    m_siblings.clear();
}

void XltlNode::replaceBy(XltlNode* node)
{
    node->m_parent = m_parent;
    node->m_symtab = m_symtab;

    if (m_parent)
    {
        for (std::vector<XltlNode*>::iterator it = m_parent->m_siblings.begin();
             it != m_parent->m_siblings.end(); ++it)
        {
            if (*it == this)
                *it = node;
        }
    }

    m_parent = nullptr;
    m_symtab = nullptr;

    if (m_prev)
        m_prev->m_next = node;
    node->m_prev = m_prev;
    m_prev = nullptr;

    m_next = nullptr;
    m_prev = nullptr;
}

XltlNode*& XltlNode::M_last()
{
    if (m_next)
        return m_next->M_last();

    return m_next;
}

#define CAT(a, b, c) a ## b ## c
#define ATTR(type, name)
#define FLAGS(mask) (mask)
#define DEF_NODE(name, ...) \
    void CAT(Xltl, name, Node)::accept(AbstractXltlNodeVisitor* v) \
    { v->visit(this); } \
    XltlNode::Flags CAT(Xltl, name, Node)::flags() const \
    { return (Flags) __VA_ARGS__; }
#define DEF_FLAG(name, value)
#include "lang/ast_xltl_nodes.def"
#undef DEF_FLAG
#undef FLAGS
#undef ATTR
#undef DEF_NODE
#undef CAT
