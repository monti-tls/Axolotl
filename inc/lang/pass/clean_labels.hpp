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

#ifndef __AXOLOTL_LANG_PASS_CLEAN_LABELS_H__
#define __AXOLOTL_LANG_PASS_CLEAN_LABELS_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace pass
    {
        //! Stage   : IR -> IR
        //! Modifies: merges `IR_Label' nodes
        //! This pass merges any duplicate `IR_Label' nodes
        class CleanLabels : public ast::NodeVisitor
        {
        public:
            using NodeVisitor::NodeVisitor;
            virtual ~CleanLabels();

            void visit(ast::IR_LabelNode* node);
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_CLEAN_LABELS_H__
