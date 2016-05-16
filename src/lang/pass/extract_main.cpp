#include "lang/pass/extract_main.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"
#include "lang/std_names.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

ExtractMain::~ExtractMain()
{
    // The Prog node should be empty now
    m_prog->addSibling(m_main);
}

void ExtractMain::init()
{ m_main = nullptr; }

void ExtractMain::visit(ProgNode* node)
{
    m_prog = node;

    m_main = new FunDeclNode(node->startToken());
    m_main->name = std_main;

    node->siblings()[0]->accept(this);
}

void ExtractMain::visit(FunDeclNode* node)
{
    Node* next = node->next();

    node->remove();
    m_main->chain(node);

    if (next)
        next->accept(this);
}

void ExtractMain::visitDefault(Node* node)
{
    Node* next = node->next();

    node->remove();

    if (m_main->siblings().size())
        m_main->siblings()[0]->chain(node);
    else
        m_main->addSibling(node);

    if (next)
        next->accept(this);
}
