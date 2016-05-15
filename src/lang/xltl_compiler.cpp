#include "lang/xltl_compiler.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/xltl_parser.hpp"
#include "lang/pass_expr_result_check.hpp"
#include "lang/pass_bind_names.hpp"
#include "lang/pass_resolve_names.hpp"
#include "lang/pass_resolve_consts.hpp"
#include "lang/pass_generate_rvalue.hpp"
#include "lang/pass_generate_lvalue.hpp"
#include "lang/pass_generate_ir.hpp"
#include "lang/pass_pretty_print.hpp"

#include "lang/ast_xltl_node_visitor.hpp"
#include "lang/ast_xltl_node_generator.hpp"

#include "lang/ast_xltl_node_visitor_impl.hpp"
#include "lang/ast_xltl_node_generator_impl.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

XltlCompiler::XltlCompiler(std::istream& in)
    : m_in(in)
    , m_parser(nullptr)
    , m_root(nullptr)
{}

XltlCompiler::~XltlCompiler()
{
    delete m_root;
    delete m_parser;
}

void XltlCompiler::compile()
{
    M_parse();
    M_pass();
}

void XltlCompiler::prettyPrint(std::ostream& os)
{
    XltlNodeVisitor::apply<PrettyPrint>(m_root, m_parser, os);
}

void XltlCompiler::M_parse()
{
    if (m_root)
        delete m_root;

    if (m_parser)
        delete m_parser;

    m_parser = new XltlParser(m_in);
    m_root = m_parser->parse();
}

void XltlCompiler::M_pass()
{
    XltlNodeVisitor::apply<ExprResultCheck>(m_root, m_parser);

    XltlNodeVisitor::apply<BindNames>(m_root, m_parser);
    XltlNodeVisitor::apply<ResolveNames>(m_root, m_parser);
    XltlNodeVisitor::apply<ResolveConsts>(m_root, m_parser);

    prettyPrint(std::cout);

    XltlNodeGenerator::transform<GenerateIR>(m_root, m_parser);

    prettyPrint(std::cout);
}
