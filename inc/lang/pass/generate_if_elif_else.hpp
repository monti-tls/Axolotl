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

#ifndef __AXOLOTL_LANG_PASS_GENERATE_IF_ELIF_ELSE_H__
#define __AXOLOTL_LANG_PASS_GENERATE_IF_ELIF_ELSE_H__

#include "lang/forward.hpp"
#include "lang/ast/node_generator.hpp"

#include <string>

namespace lang
{
    namespace pass
    {
        class GenerateIfElifElse : public ast::NodeGenerator
        {
        public:
            using NodeGenerator::NodeGenerator;
            ~GenerateIfElifElse();

            void visit(ast::IfNode* node);
            void visit(ast::ElifNode* node);
            void visit(ast::ElseNode* node);
            void visitDefault(ast::Node* node);

        private:
            ast::Node* m_last_node;
            std::string m_end_label;
            bool m_stop = false;
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_GENERATE_IF_ELIF_ELSE_H__
