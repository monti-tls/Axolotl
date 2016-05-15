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

#ifndef __AXOLOTL_LANG_PASS_GENERATE_IR_H__
#define __AXOLOTL_LANG_PASS_GENERATE_IR_H__

#include "lang/forward.hpp"
#include "lang/ast_xltl_node_generator.hpp"

namespace lang
{
    namespace pass
    {
        class GenerateIR : public ast::XltlNodeGenerator
        {
        public:
            using XltlNodeGenerator::XltlNodeGenerator;
            ~GenerateIR();

            void visit(ast::XltlProgNode* node);
            void visit(ast::XltlFunDeclNode* node);
            void visit(ast::XltlReturnNode* node);
            void visit(ast::XltlAssignNode* node);
            void visitDefault(ast::XltlNode* node);
        };
    }
}

#endif // __AXOLOTL_LANG_PASS_GENERATE_IR_H__
