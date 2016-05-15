#include "lang/pass_pretty_print.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

PrettyPrint::PrettyPrint(Parser* parser, std::ostream& os)
    : XltlNodeVisitor(parser)
    , m_os(os)
    , m_indent(0)
{}

PrettyPrint::~PrettyPrint()
{}

void PrettyPrint::visitDefault(XltlNode* node)
{
    for (auto sib : node->siblings())
        sib->accept(this);
    M_follow(node);
}

void PrettyPrint::visit(XltlNameNode* node)
{
    M_indent();
    m_os << "(Name: " << node->value << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlConstNode* node)
{
    std::string classname = node->value.classname();
    std::string serialized = node->value.method(std_serialize, {}).unwrap<std::string>();

    M_indent();
    m_os << "(Const: " << classname << "=\"" << serialized << "\")";

    M_follow(node);
}

void PrettyPrint::visit(XltlInvokeNode* node)
{
    M_indent();
    m_os << "(Invoke" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);

    if (node->siblings().size() > 1)
    {
        m_os << "," << std::endl;
        node->siblings()[1]->accept(this);
    }
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlMemberNode* node)
{
    M_indent();
    m_os << "(Member: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlMethodNode* node)
{
    M_indent();
    m_os << "(Method: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    if (node->siblings().size() > 1)
    {
        m_os << "," << std::endl;
        node->siblings()[1]->accept(this);
    }
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlAssignNode* node)
{
    M_indent();
    m_os << "(Assign" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << "," << std::endl;
    node->siblings()[1]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlIfNode* node)
{
    M_indent();
    m_os << "(If" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << "," << std::endl;
    node->siblings()[1]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlElifNode* node)
{
    M_indent();
    m_os << "(Elif" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << "," << std::endl;
    node->siblings()[1]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlElseNode* node)
{
    M_indent();
    m_os << "(Else" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlWhileNode* node)
{
    M_indent();
    m_os << "(While" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << "," << std::endl;
    node->siblings()[1]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlReturnNode* node)
{
    M_indent();
    m_os << "(Return" << std::endl;
    ++m_indent;
    if (node->siblings().size())
        node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlFunDeclNode* node)
{
    M_indent();
    m_os << "(FunDecl: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlProgNode* node)
{
    M_indent();
    m_os << "(Prog" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlGlobalRefNode* node)
{
    M_indent();
    m_os << "(GlobalRef: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlLocalRefNode* node)
{
    M_indent();
    m_os << "(LocalRef: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlConstRefNode* node)
{
    M_indent();
    m_os << "(ConstRef: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_ProgNode* node)
{
    M_indent();
    m_os << "(IR_Prog" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_FunDeclNode* node)
{
    M_indent();
    m_os << "(IR_FunDecl: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_LoadConstNode* node)
{
    M_indent();
    m_os << "(IR_LoadConst: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_LoadGlobalNode* node)
{
    M_indent();
    m_os << "(IR_LoadGlobal: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_StorGlobalNode* node)
{
    M_indent();
    m_os << "(IR_StorGlobal: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_LoadLocalNode* node)
{
    M_indent();
    m_os << "(IR_LoadLocal: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_StorLocalNode* node)
{
    M_indent();
    m_os << "(IR_StorLocal: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_LoadMemberNode* node)
{
    M_indent();
    m_os << "(IR_LoadMember: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_StorMemberNode* node)
{
    M_indent();
    m_os << "(IR_StorMember: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_LabelNode* node)
{
    M_indent();
    m_os << "(IR_Label: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_GotoNode* node)
{
    M_indent();
    m_os << "(IR_Goto: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_GotoIfNode* node)
{
    M_indent();
    m_os << "(IR_GotoIf: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_InvokeNode* node)
{
    M_indent();
    m_os << "(IR_Invoke: " << node->argc << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_MethodNode* node)
{
    M_indent();
    m_os << "(IR_Method: " << node->name << ", " << node->argc << ")";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_ReturnNode* node)
{
    M_indent();
    m_os << "(IR_Return)";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_LeaveNode* node)
{
    M_indent();
    m_os << "(IR_Leave)";

    M_follow(node);
}

void PrettyPrint::visit(XltlIR_PopNode* node)
{
    M_indent();
    m_os << "(IR_Pop)";

    M_follow(node);
}

void PrettyPrint::M_follow(XltlNode* node)
{
    if (node->next())
    {
        m_os << " ->" << std::endl;
        node->next()->accept(this);
    }
}

void PrettyPrint::M_indent()
{
    for (int i = 0; i < m_indent; ++i)
        m_os << "    ";
}