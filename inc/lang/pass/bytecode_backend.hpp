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

#ifndef __AXOLOTL_LANG_PASS_BYTECODE_BACKEND_H__
#define __AXOLOTL_LANG_PASS_BYTECODE_BACKEND_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"
#include "bits/bits.hpp"

namespace lang
{
    namespace pass
    {
        class ByteCodeBackend : public ast::NodeVisitor
        {
        public:
            ByteCodeBackend(ParserBase* parser);
            virtual ~ByteCodeBackend();

            void finalize();
            bits::Blob const& blob() const;

            void visit(ast::IR_ProgNode* node);
            void visit(ast::IR_FunDeclNode* node);
            void visit(ast::IR_LoadConstNode* node);
            void visit(ast::IR_LoadGlobalNode* node);
            void visit(ast::IR_StorGlobalNode* node);
            void visit(ast::IR_LoadLocalNode* node);
            void visit(ast::IR_StorLocalNode* node);
            void visit(ast::IR_LoadMemberNode* node);
            void visit(ast::IR_StorMemberNode* node);
            void visit(ast::IR_LabelNode* node);
            void visit(ast::IR_GotoNode* node);
            void visit(ast::IR_GotoIfTrueNode* node);
            void visit(ast::IR_GotoIfFalseNode* node);
            void visit(ast::IR_InvokeNode* node);
            void visit(ast::IR_MethodNode* node);
            void visit(ast::IR_ReturnNode* node);
            void visit(ast::IR_LeaveNode* node);
            void visit(ast::IR_PopNode* node);
            void visit(ast::IR_ImportNode* node);
            void visit(ast::IR_ImportMaskNode* node);

            void visitDefault(ast::Node* node);

        private:
            bits::Assembler* m_assembler;
            bits::Blob m_blob;
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_BYTECODE_BACKEND_H__
