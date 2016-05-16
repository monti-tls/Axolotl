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

#ifndef __AXOLOTL_LANG_PASS_EXTRACT_MAIN_H__
#define __AXOLOTL_LANG_PASS_EXTRACT_MAIN_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace pass
    {
        //! Stage   : Ast -> Ast
        //! Modifies: The root node and non-nested FunDecl nodes
        //! This pass extracts the content of the ProgNode into a new
        //!   FunDecl node that is linked with other FunDecl nodes.
        class ExtractMain : public ast::NodeVisitor
        {
        public:
            using NodeVisitor::NodeVisitor;
            virtual ~ExtractMain();

            void init();

            void visit(ast::ProgNode* node);
            void visit(ast::FunDeclNode* node);
            void visitDefault(ast::Node* node);

        private:
            ast::ProgNode* m_prog;
            ast::FunDeclNode* m_main;
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_EXTRACT_MAIN_H__
