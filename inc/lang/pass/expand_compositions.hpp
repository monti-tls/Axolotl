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

#ifndef __AXOLOTL_LANG_PASS_EXPAND_COMPOSITIONS_H__
#define __AXOLOTL_LANG_PASS_EXPAND_COMPOSITIONS_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace pass
    {
        //! Stage   : AST -> AST
        //! Modifies: all Compose nodes
        //! This pass replaces function composition nodes with the
        //!   appropriate library function call
        class ExpandCompositions : public ast::NodeVisitor
        {
        public:
            using NodeVisitor::NodeVisitor;
            ~ExpandCompositions();

            void visit(ast::ComposeNode* node);
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_EXPAND_COMPOSITIONS_H__
