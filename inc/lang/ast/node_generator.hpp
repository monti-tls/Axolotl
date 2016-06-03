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

#include <string>

namespace lang
{
    namespace ast
    {
        class NodeGenerator;

        class AbstractNodeGenerator : public NodeVisitor
        {
        public:
            AbstractNodeGenerator(ParserBase* parser, bool nofollow = false);
            AbstractNodeGenerator(AbstractNodeGenerator* gen, bool nofollow = false);
            virtual ~AbstractNodeGenerator();

            #define CAT(a, b) a ## b
            #define DEF_NODE(name, ...) \
                virtual void hook(NodeGenerator* gen, CAT(name, Node)* node) = 0;
            #define DEF_FLAG(name, value)
            #include "lang/ast/nodes.def"
            #undef DEF_FLAG
            #undef DEF_NODE
            #undef CAT

        protected:
            Node* m_generated;
            int m_label_alloc;
            int* m_label_alloc_ptr;
        };

        class NodeGenerator : public AbstractNodeGenerator
        {
        public:
            using AbstractNodeGenerator::AbstractNodeGenerator;
            virtual ~NodeGenerator();

            virtual void init();
            Node* generated() const;

            #define CAT(a, b) a ## b
            #define DEF_NODE(name, ...) \
                virtual void hook(NodeGenerator* gen, CAT(name, Node)* node);
            #define DEF_FLAG(name, value)
            #include "lang/ast/nodes.def"
            #undef DEF_FLAG
            #undef DEF_NODE
            #undef CAT
                
            std::string newLabel();
            void emit(Node* node);
            void emitLabel(Node* parent, std::string const& name);
            void emitGoto(Node* parent, std::string const& name);
            void emitGotoIfTrue(Node* parent, std::string const& name);
            void emitGotoIfFalse(Node* parent, std::string const& name);

        public:
            template <typename Generator, typename... Args>
            static Node* generate(Node* node, Args&&... args);

            template <typename Generator, typename... Args>
            static void transform(Node*& node, Args&&... args);
        };
    }
}

#endif // __AXOLOTL_LANG_AST_NODE_GENERATOR_H__
