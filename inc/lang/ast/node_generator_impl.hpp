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

#ifndef __AXOLOTL_LANG_AST_NODE_GENERATOR_IMPL_H__
#define __AXOLOTL_LANG_AST_NODE_GENERATOR_IMPL_H__

#include "lang/ast/node_visitor.hpp"
#include "lang/ast/node.hpp"

#include <utility>

namespace lang
{
    namespace ast
    {
        template <typename Generator, typename... Args>
        Node* NodeGenerator::generate(Node* node, Args&&... args)
        {
            Generator* generator = new Generator(std::forward<Args>(args)...);
            generator->init();
            node->accept(generator);
            Node* gen = generator->generated();
            delete generator;
            return gen;
        }

        template <typename Generator, typename... Args>
        void NodeGenerator::transform(Node*& node, Args&&... args)
        {
            Node* old_node = node;
            node = generate<Generator>(node, std::forward<Args>(args)...);
            delete old_node;
        }
    }
}

#endif // __AXOLOTL_LANG_AST_NODE_GENERATOR_IMPL_H__
