#include "lang/ast/node_generator.hpp"
#include "lang/ast/node.hpp"

#include <sstream>

using namespace lang;
using namespace ast;

AbstractNodeGenerator::AbstractNodeGenerator(ParserBase* parser, bool nofollow)
    : NodeVisitor(parser, nofollow)
    , m_label_alloc(0)
    , m_label_alloc_ptr(&m_label_alloc)
{}

AbstractNodeGenerator::AbstractNodeGenerator(AbstractNodeGenerator* gen, bool nofollow)
    : NodeVisitor(gen, nofollow)
    , m_label_alloc(-1)
    , m_label_alloc_ptr(gen->m_label_alloc_ptr)
{}

AbstractNodeGenerator::~AbstractNodeGenerator()
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

#define CAT(a, b) a ## b
#define DEF_FLAG(name, value)
#define DEF_NODE(name, ...) \
    void NodeGenerator::hook(NodeGenerator* gen, CAT(name, Node)* node) \
    { \
        if (parent()) \
        { \
            NodeGenerator* up = dynamic_cast<NodeGenerator*>(parent()); \
            if (up) \
                up->hook(gen, node); \
        } \
    }
#include "lang/ast/nodes.def"
#undef DEF_NODE
#undef DEF_FLAG
#undef CAT

std::string NodeGenerator::newLabel()
{
    std::ostringstream ss;
    ss << "l" << (*m_label_alloc_ptr)++;
    return ss.str();
}

void NodeGenerator::emit(Node* node)
{
    if (m_generated)
        m_generated->chain(node);
    else
        m_generated = node;
}

void NodeGenerator::emitLabel(Node* parent, std::string const& name)
{
    IR_LabelNode* node = new IR_LabelNode(parent->startToken());
    node->name = name;
    emit(node);
}

void NodeGenerator::emitGoto(Node* parent, std::string const& name)
{
    IR_GotoNode* node = new IR_GotoNode(parent->startToken());
    node->name = name;
    emit(node);
}

void NodeGenerator::emitGotoIfTrue(Node* parent, std::string const& name)
{
    IR_GotoIfTrueNode* node = new IR_GotoIfTrueNode(parent->startToken());
    node->name = name;
    emit(node);
}

void NodeGenerator::emitGotoIfFalse(Node* parent, std::string const& name)
{
    IR_GotoIfFalseNode* node = new IR_GotoIfFalseNode(parent->startToken());
    node->name = name;
    emit(node);
}
