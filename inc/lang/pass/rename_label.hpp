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

#ifndef __AXOLOTL_LANG_PASS_RENAME_LABEL_H__
#define __AXOLOTL_LANG_PASS_RENAME_LABEL_H__

#include "lang/forward.hpp"
#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace pass
    {
        //! Stage   : IR -> IR
        //! Modifies: some `IR_Label' nodes
        //! This pass renames a label node
        class RenameLabel : public ast::NodeVisitor
        {
        public:
            RenameLabel(ParserBase* parent, std::string const& from, std::string const& to, bool nofollow = false);
            RenameLabel(AbstractNodeVisitor* parent, std::string const& from, std::string const& to, bool nofollow = false);
            virtual ~RenameLabel();

            void visit(ast::IR_GotoNode* node);
            void visit(ast::IR_GotoIfTrueNode* node);
            void visit(ast::IR_GotoIfFalseNode* node);

        private:
            std::string m_from;
            std::string m_to;
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_RENAME_LABEL_H__
