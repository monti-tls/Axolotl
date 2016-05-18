#include "lang/pass/pretty_print.hpp"
#include "lang/ast/node.hpp"
#include "lang/ast/node_visitor.hpp"

using namespace lang;
using namespace ast;
using namespace pass;

PrettyPrint::PrettyPrint(ParserBase* parser, std::ostream& os)
    : NodeVisitor(parser)
    , m_os(os)
    , m_indent(0)
{}

PrettyPrint::~PrettyPrint()
{}

void PrettyPrint::visitDefault(Node* node)
{
    for (auto sib : node->siblings())
        sib->accept(this);
    M_follow(node);
}

void PrettyPrint::visit(NameNode* node)
{
    M_indent();
    m_os << "(Name: " << node->value << ")";

    M_follow(node);
}

void PrettyPrint::visit(ConstNode* node)
{
    std::string classname = node->value.classname();
    std::string serialized = node->value.method(std_serialize, {}).unwrap<std::string>();

    M_indent();
    m_os << "(Const: " << classname << "=\"" << serialized << "\")";

    M_follow(node);
}

void PrettyPrint::visit(InvokeNode* node)
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

void PrettyPrint::visit(MemberNode* node)
{
    M_indent();
    m_os << "(Member: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(MethodNode* node)
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

void PrettyPrint::visit(AssignNode* node)
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

void PrettyPrint::visit(IfNode* node)
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

void PrettyPrint::visit(ElifNode* node)
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

void PrettyPrint::visit(ElseNode* node)
{
    M_indent();
    m_os << "(Else" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(WhileNode* node)
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

void PrettyPrint::visit(ReturnNode* node)
{
    M_indent();
    m_os << "(Return";
    if (node->siblings().size())
    {
        m_os << std::endl;
        ++m_indent;
        node->siblings()[0]->accept(this);
        --m_indent;
    }
    m_os << ")";

    M_follow(node);
}

void PrettyPrint::visit(ImportNode* node)
{
    M_indent();
    m_os << "(Import: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(ImportMaskNode* node)
{
    M_indent();
    m_os << "(ImportMask: " << node->name << "." << node->mask << ")";

    M_follow(node);
}

void PrettyPrint::visit(FunDeclNode* node)
{
    M_indent();
    m_os << "(FunDecl: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(ClassDeclNode* node)
{
    M_indent();
    m_os << "(ClassDecl: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(ProgNode* node)
{
    M_indent();
    m_os << "(Prog" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(GlobalRefNode* node)
{
    M_indent();
    m_os << "(GlobalRef: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(LocalRefNode* node)
{
    M_indent();
    m_os << "(LocalRef: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(ConstRefNode* node)
{
    M_indent();
    m_os << "(ConstRef: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_ProgNode* node)
{
    M_indent();
    m_os << "(IR_Prog" << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(IR_FunDeclNode* node)
{
    M_indent();
    m_os << "(IR_FunDecl: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(IR_ClassDeclNode* node)
{
    M_indent();
    m_os << "(IR_ClassDecl: " << node->name << std::endl;
    ++m_indent;
    node->siblings()[0]->accept(this);
    m_os << ")";
    --m_indent;

    M_follow(node);
}

void PrettyPrint::visit(IR_LoadConstNode* node)
{
    M_indent();
    m_os << "(IR_LoadConst: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_LoadGlobalNode* node)
{
    M_indent();
    m_os << "(IR_LoadGlobal: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_StorGlobalNode* node)
{
    M_indent();
    m_os << "(IR_StorGlobal: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_LoadLocalNode* node)
{
    M_indent();
    m_os << "(IR_LoadLocal: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_StorLocalNode* node)
{
    M_indent();
    m_os << "(IR_StorLocal: " << node->index << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_LoadMemberNode* node)
{
    M_indent();
    m_os << "(IR_LoadMember: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_StorMemberNode* node)
{
    M_indent();
    m_os << "(IR_StorMember: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_LabelNode* node)
{
    M_indent();
    m_os << "(IR_Label: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_GotoNode* node)
{
    M_indent();
    m_os << "(IR_Goto: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_GotoIfTrueNode* node)
{
    M_indent();
    m_os << "(IR_GotoIfTrue: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_GotoIfFalseNode* node)
{
    M_indent();
    m_os << "(IR_GotoIfFalse: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_InvokeNode* node)
{
    M_indent();
    m_os << "(IR_Invoke: " << node->argc << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_MethodNode* node)
{
    M_indent();
    m_os << "(IR_Method: " << node->name << ", " << node->argc << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_ReturnNode* node)
{
    M_indent();
    m_os << "(IR_Return)";

    M_follow(node);
}

void PrettyPrint::visit(IR_LeaveNode* node)
{
    M_indent();
    m_os << "(IR_Leave)";

    M_follow(node);
}

void PrettyPrint::visit(IR_PopNode* node)
{
    M_indent();
    m_os << "(IR_Pop)";

    M_follow(node);
}

void PrettyPrint::visit(IR_ImportNode* node)
{
    M_indent();
    m_os << "(IR_Import: " << node->name << ")";

    M_follow(node);
}

void PrettyPrint::visit(IR_ImportMaskNode* node)
{
    M_indent();
    m_os << "(IR_ImportMask: " << node->name << "." << node->mask << ")";

    M_follow(node);
}

void PrettyPrint::M_follow(Node* node)
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
