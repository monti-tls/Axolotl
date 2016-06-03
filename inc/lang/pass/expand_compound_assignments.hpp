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

#ifndef __AXOLOTL_LANG_PASS_EXPAND_COMPOUND_ASSIGNMENTS_H__
#define __AXOLOTL_LANG_PASS_EXPAND_COMPOUND_ASSIGNMENTS_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace pass
    {
        //! Stage   : AST -> AST
        //! Modifies: some Assign nodes
        //! This pass replaces compound assignments (i.e. +=, -=, %= or whatever)
        //!   into a simple assignment and the appropriate method call
        class ExpandCompoundAssignments : public ast::NodeVisitor
        {
        public:
            using NodeVisitor::NodeVisitor;
            ~ExpandCompoundAssignments();

            void visit(ast::AssignNode* node);
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_EXPAND_COMPOUND_ASSIGNMENTS_H__
