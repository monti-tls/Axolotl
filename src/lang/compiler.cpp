#include "lang/compiler.hpp"
#include "lang/parser.hpp"
#include "lang/pass/pass.hpp"

#include "lang/ast/ast.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

Compiler::Compiler(std::istream& in)
    : m_in(in)
    , m_parser(nullptr)
    , m_root(nullptr)
{}

Compiler::~Compiler()
{
    delete m_root;
    delete m_parser;
}

void Compiler::compile()
{
    M_parse();
    M_pass();
}

void Compiler::prettyPrint(std::ostream& os)
{
    NodeVisitor::apply<PrettyPrint>(m_root, m_parser, os);
}

void Compiler::M_parse()
{
    if (m_root)
        delete m_root;

    if (m_parser)
        delete m_parser;

    m_parser = new Parser(m_in);
    m_root = m_parser->parse();
}

void Compiler::M_pass()
{
    NodeVisitor::apply<ExprResultCheck>(m_root, m_parser);

    NodeVisitor::apply<BindNames>(m_root, m_parser);
    NodeVisitor::apply<ResolveNames>(m_root, m_parser);
    NodeVisitor::apply<ResolveConsts>(m_root, m_parser);

    prettyPrint(std::cout);

    NodeGenerator::transform<GenerateIR>(m_root, m_parser);

    prettyPrint(std::cout);
}
