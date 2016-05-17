#include "lang/compiler.hpp"
#include "lang/parser.hpp"
#include "lang/pass/pass.hpp"

#include "lang/ast/ast.hpp"
#include "bits/bits.hpp"

using namespace lang;
using namespace ast;
using namespace pass;
using namespace bits;
using namespace vm;

Compiler::Compiler(std::string module_name, std::istream& in, int flags, std::ostream& out)
    : m_in(in)
    , m_flags((Compiler::Flags) flags)
    , m_out(out)
    , m_parser(nullptr)
    , m_root(nullptr)
    , m_module(module_name)
{}

Compiler::Compiler(Module const& module, std::istream& in, int flags, std::ostream& out)
    : m_in(in)
    , m_flags((Compiler::Flags) flags)
    , m_out(out)
    , m_parser(nullptr)
    , m_root(nullptr)
    , m_module(module)
{}

Compiler::~Compiler()
{
    delete m_root;
    delete m_parser;
}

Module Compiler::compile()
{
    M_parse();

    if (m_flags & PP_EARLY_AST)
        M_prettyPrint();

    M_transformAST();

    if (m_flags & PP_AST)
        M_prettyPrint();

    M_generateIR();

    if (m_flags & PP_IR)
        M_prettyPrint();

    Blob blob = M_byteCodeBackend();

    if (m_flags & DIS_BYTECODE)
    {
        Disassembler dis(blob, m_out);
        dis.dumpAll();
    }

    m_module.setBlob(blob);

    return m_module;
}

void Compiler::M_parse()
{
    if (m_root)
        delete m_root;

    if (m_parser)
        delete m_parser;

    m_parser = new Parser(m_in, m_module);
    m_root = m_parser->parse();
}

void Compiler::M_transformAST()
{
    NodeVisitor::apply<ExprResultCheck>(m_root, m_parser);
    NodeVisitor::apply<ExtractMain>(m_root, m_parser);
    NodeVisitor::apply<AddImplicitReturn>(m_root, m_parser);
    NodeVisitor::apply<BindNames>(m_root, m_parser);
    NodeVisitor::apply<ResolveNames>(m_root, m_parser);
    NodeVisitor::apply<ResolveConsts>(m_root, m_parser);
}

void Compiler::M_generateIR()
{
    NodeGenerator::transform<GenerateIR>(m_root, m_parser);
    NodeVisitor::apply<CleanLabels>(m_root, m_parser);
}

Blob Compiler::M_byteCodeBackend()
{
    ByteCodeBackend* backend = new ByteCodeBackend(m_parser);
    m_root->accept(backend);

    backend->finalize();
    Blob blob = backend->blob();
    delete backend;

    return blob;
}

void Compiler::M_prettyPrint()
{
    NodeVisitor::apply<PrettyPrint>(m_root, m_parser, m_out);
    m_out << std::endl;
}
