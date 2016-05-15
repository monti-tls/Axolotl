/*  This file is part of Axolotl.
 *
 * Axolotl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Axolotl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Axolotl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AXOLOTL_LANG_PASS_PRETTY_PRINT_H__
#define __AXOLOTL_LANG_PASS_PRETTY_PRINT_H__

#include "lang/forward.hpp"
#include "lang/ast_xltl_node_visitor.hpp"

#include <iostream>

namespace lang
{
    namespace pass
    {
        class PrettyPrint : public ast::XltlNodeVisitor
        {
        public:
            PrettyPrint(Parser* parser, std::ostream& os);
            ~PrettyPrint();

            void visitDefault(ast::XltlNode* node);
            void visit(ast::XltlNameNode* node);
            void visit(ast::XltlConstNode* node);
            void visit(ast::XltlInvokeNode* node);
            void visit(ast::XltlMemberNode* node);
            void visit(ast::XltlMethodNode* node);
            void visit(ast::XltlAssignNode* node);
            void visit(ast::XltlIfNode* node);
            void visit(ast::XltlElifNode* node);
            void visit(ast::XltlElseNode* node);
            void visit(ast::XltlWhileNode* node);
            void visit(ast::XltlReturnNode* node);
            void visit(ast::XltlFunDeclNode* node);
            void visit(ast::XltlProgNode* node);
            void visit(ast::XltlGlobalRefNode* node);
            void visit(ast::XltlLocalRefNode* node);
            void visit(ast::XltlConstRefNode* node);
            void visit(ast::XltlIR_ProgNode* node);
            void visit(ast::XltlIR_FunDeclNode* node);
            void visit(ast::XltlIR_LoadConstNode* node);
            void visit(ast::XltlIR_LoadGlobalNode* node);
            void visit(ast::XltlIR_StorGlobalNode* node);
            void visit(ast::XltlIR_LoadLocalNode* node);
            void visit(ast::XltlIR_StorLocalNode* node);
            void visit(ast::XltlIR_LoadMemberNode* node);
            void visit(ast::XltlIR_StorMemberNode* node);
            void visit(ast::XltlIR_LabelNode* node);
            void visit(ast::XltlIR_GotoNode* node);
            void visit(ast::XltlIR_GotoIfNode* node);
            void visit(ast::XltlIR_InvokeNode* node);
            void visit(ast::XltlIR_MethodNode* node);
            void visit(ast::XltlIR_ReturnNode* node);
            void visit(ast::XltlIR_LeaveNode* node);
            void visit(ast::XltlIR_PopNode* node);

        private:
            void M_follow(ast::XltlNode* node);
            void M_indent();

        private:
            std::ostream& m_os;
            int m_indent;
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_PRETTY_PRINT_H__
