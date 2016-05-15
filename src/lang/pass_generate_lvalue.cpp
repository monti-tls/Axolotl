#include "lang/pass_generate_lvalue.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"
#include "lang/ast_xltl_node_generator.hpp"
#include "lang/pass_generate_rvalue.hpp"

#include "lang/ast_xltl_node_visitor_impl.hpp"
#include "lang/ast_xltl_node_generator_impl.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

GenerateLValue::~GenerateLValue() {}

void GenerateLValue::visit(XltlGlobalRefNode* node)
{
    XltlIR_StorGlobalNode* new_node = new XltlIR_StorGlobalNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
}

void GenerateLValue::visit(XltlLocalRefNode* node)
{
    XltlIR_StorLocalNode* new_node = new XltlIR_StorLocalNode(node->startToken());
    new_node->index = node->index;

    M_emit(new_node);
}

void GenerateLValue::visit(XltlMemberNode* node)
{
    M_emit(XltlNodeGenerator::generate<GenerateRValue>(node->siblings()[0], this));

    XltlIR_StorMemberNode* new_node = new XltlIR_StorMemberNode(node->startToken());
    new_node->name = node->name;

    M_emit(new_node);
}

void GenerateLValue::visitDefault(XltlNode* node)
{ M_error(node, "not an lvalue"); }
