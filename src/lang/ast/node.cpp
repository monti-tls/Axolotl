#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;

Node::Node(Token const& start_token)
    : m_start_token(start_token)
    , m_prev(nullptr)
    , m_next(nullptr)
    , m_parent(nullptr)
    , m_symtab(nullptr)
{}

Node::~Node()
{
    if (m_symtab)
        delete m_symtab;

    if (m_next)
        delete m_next;
    
    for (auto sib : m_siblings)
        delete sib;
}

Token const& Node::startToken() const
{ return m_start_token; }

std::vector<Node*> const& Node::siblings() const
{ return m_siblings; }

void Node::addSibling(Node* sibling)
{
    if (sibling)
    {
        sibling->setParent(this);
        m_siblings.push_back(sibling);
    }
}

void Node::chain(Node* node)
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

Node* Node::prev() const
{ return m_prev; }

Node* Node::next() const
{ return m_next; }

Node* Node::last() const
{
    if (!m_next)
        return const_cast<Node*>(this);

    return m_next->last();
}

std::size_t Node::chainLength() const
{
    std::size_t i = 0;
    for (Node const* node = this; node; node = node->m_next)
        ++i;
    return i;
}

Node* Node::root() const
{
    if (m_parent)
        return m_parent->root();

    return const_cast<Node*>(this);
}

void Node::setParent(Node* parent)
{
    m_parent = parent;

    if (m_next)
        m_next->setParent(parent);
}

Node* Node::parent() const
{ return m_parent; }

void Node::attachSymtab(Symtab* symtab)
{
    if (m_symtab)
        delete m_symtab;

    m_symtab = symtab;
}

Symtab* Node::detachSymtab()
{
    Symtab* symtab = m_symtab;
    m_symtab = nullptr;
    return symtab;
}

Symtab* Node::symtab() const
{
    if (m_symtab)
        return m_symtab;
    else if (m_parent)
        return m_parent->symtab();

    return nullptr;
}

void Node::substituteWith(Node* node)
{
    node->setParent(m_parent);
    node->m_symtab = m_symtab;

    if (m_parent)
    {
        for (std::vector<Node*>::iterator it = m_parent->m_siblings.begin();
             it != m_parent->m_siblings.end(); ++it)
        {
            if (*it == this)
                *it = node;
        }
    }

    m_parent = nullptr;
    m_symtab = nullptr;
}

void Node::exchangeWith(Node* node)
{
    node->setParent(m_parent);
    node->m_symtab = m_symtab;

    if (m_parent)
    {
        for (std::vector<Node*>::iterator it = m_parent->m_siblings.begin();
             it != m_parent->m_siblings.end(); ++it)
        {
            if (*it == this)
                *it = node;
        }
    }

    for (auto sib : m_siblings)
        sib->setParent(node);

    m_parent = nullptr;
    m_symtab = nullptr;

    Node*& last = node->M_last();

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

void Node::replaceBy(Node* node)
{
    node->setParent(m_parent);
    node->m_symtab = m_symtab;

    if (m_parent)
    {
        for (std::vector<Node*>::iterator it = m_parent->m_siblings.begin();
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

void Node::removeFromChain()
{
    if (m_prev)
        m_prev->m_next = m_next;
    if (m_next)
        m_next->m_prev = m_prev;

    m_prev = nullptr;
    m_next = nullptr;
}

void Node::remove()
{
    if (m_parent)
    {
        for (std::vector<Node*>::iterator it = m_parent->m_siblings.begin();
             it != m_parent->m_siblings.end(); ++it)
        {
            if (*it == this)
            {
                m_parent->m_siblings.erase(it);
                break;
            }
        }
    }

    m_parent = nullptr;

    removeFromChain();
}

Node*& Node::M_last()
{
    if (m_next)
        return m_next->M_last();

    return m_next;
}

#define CAT(a, b) a ## b
#define ATTR(type, name)
#define FLAGS(mask) (mask)
#define DEF_NODE(name, ...) \
    void CAT(name, Node)::accept(AbstractNodeVisitor* v) \
    { v->visit(this); } \
    Node::Flags CAT(name, Node)::flags() const \
    { return (Flags) __VA_ARGS__; }
#define DEF_FLAG(name, value)
#include "lang/ast/nodes.def"
#undef DEF_FLAG
#undef FLAGS
#undef ATTR
#undef DEF_NODE
#undef CAT
