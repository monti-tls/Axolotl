#include "lang/ast/node_generator.hpp"
#include "lang/ast/node.hpp"

using namespace lang;
using namespace ast;

NodeGenerator::~NodeGenerator()
{}

void NodeGenerator::init()
{
    NodeVisitor::init();
    m_generated = nullptr;
}

Node* NodeGenerator::generated() const
{ return m_generated; }

void NodeGenerator::M_emit(Node* node)
{
    if (m_generated)
        m_generated->chain(node);
    else
        m_generated = node;
}
