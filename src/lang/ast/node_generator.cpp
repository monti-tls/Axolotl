#include "lang/ast/node_generator.hpp"
#include "lang/ast/node.hpp"

#include <sstream>

using namespace lang;
using namespace ast;

NodeGenerator::NodeGenerator(ParserBase* parser, bool nofollow)
    : NodeVisitor(parser, nofollow)
    , m_label_alloc(0)
    , m_label_alloc_ptr(&m_label_alloc)
{}

NodeGenerator::NodeGenerator(NodeGenerator* gen, bool nofollow)
    : NodeVisitor(gen, nofollow)
    , m_label_alloc(-1)
    , m_label_alloc_ptr(gen->m_label_alloc_ptr)
{}

NodeGenerator::~NodeGenerator()
{}

void NodeGenerator::init()
{
    NodeVisitor::init();
    m_generated = nullptr;
}

Node* NodeGenerator::generated() const
{ return m_generated; }

std::string NodeGenerator::M_newLabel()
{
    std::ostringstream ss;
    ss << "l" << (*m_label_alloc_ptr)++;
    return ss.str();
}

void NodeGenerator::M_emit(Node* node)
{
    if (m_generated)
        m_generated->chain(node);
    else
        m_generated = node;
}

void NodeGenerator::M_emitLabel(Node* parent, std::string const& name)
{
    IR_LabelNode* node = new IR_LabelNode(parent->startToken());
    node->name = name;
    M_emit(node);
}

void NodeGenerator::M_emitGoto(Node* parent, std::string const& name)
{
    IR_GotoNode* node = new IR_GotoNode(parent->startToken());
    node->name = name;
    M_emit(node);
}

void NodeGenerator::M_emitGotoIfTrue(Node* parent, std::string const& name)
{
    IR_GotoIfTrueNode* node = new IR_GotoIfTrueNode(parent->startToken());
    node->name = name;
    M_emit(node);
}

void NodeGenerator::M_emitGotoIfFalse(Node* parent, std::string const& name)
{
    IR_GotoIfFalseNode* node = new IR_GotoIfFalseNode(parent->startToken());
    node->name = name;
    M_emit(node);
}
