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

#ifndef __AXOLOTL_LANG_AST_NODE_GENERATOR_H__
#define __AXOLOTL_LANG_AST_NODE_GENERATOR_H__

#include "lang/ast/node_visitor.hpp"

namespace lang
{
    namespace ast
    {
        class NodeGenerator : public NodeVisitor
        {
        public:
            using NodeVisitor::NodeVisitor;
            virtual ~NodeGenerator();

            virtual void init();
            Node* generated() const;

        protected:
            void M_emit(Node* node);

        private:
            Node* m_generated;

        public:
            template <typename Generator, typename... Args>
            static Node* generate(Node* node, Args&&... args);

            template <typename Generator, typename... Args>
            static void transform(Node*& node, Args&&... args);
        };
    }
}

#endif // __AXOLOTL_LANG_AST_NODE_GENERATOR_H__
