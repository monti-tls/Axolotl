#include "lang/ast_xltl_node_generator.hpp"
#include "lang/ast_xltl_node.hpp"

using namespace lang;
using namespace ast;

XltlNodeGenerator::~XltlNodeGenerator()
{}

void XltlNodeGenerator::init()
{
    XltlNodeVisitor::init();
    m_generated = nullptr;
}

XltlNode* XltlNodeGenerator::generated() const
{ return m_generated; }

void XltlNodeGenerator::M_emit(XltlNode* node)
{
    if (m_generated)
        m_generated->chain(node);
    else
        m_generated = node;
}
