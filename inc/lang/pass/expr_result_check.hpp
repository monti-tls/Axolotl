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

#ifndef __AXOLOTL_LANG_PASS_EXPR_RESULT_CHECK_H__
#define __AXOLOTL_LANG_PASS_EXPR_RESULT_CHECK_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace pass
    {
        //! Stage   : AST -> AST
        //! Modifies: nothing
        //! This pass checks for unused expression results.
        //! It does so by scanning statement-level nodes, and thus
        //!   only enters into block-style nodes.
        //! It checks for name of member access alone
        class ExprResultCheck : public ast::NodeVisitor
        {
        public:
            using NodeVisitor::NodeVisitor;
            ~ExprResultCheck();

            void visit(ast::MemberNode* node);
            void visit(ast::NameNode* node);
            void visitDefault(ast::Node* node);
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_EXPR_RESULT_CHECK_H__
