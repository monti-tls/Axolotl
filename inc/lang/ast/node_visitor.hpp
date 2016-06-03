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

#ifndef __AXOLOTL_LANG_AST_NODE_VISITOR_H__
#define __AXOLOTL_LANG_AST_NODE_VISITOR_H__

#include "core/core.hpp"
#include "lang/forward.hpp"

#include <vector>
#include <string>

namespace lang
{
    namespace ast
    {
        class AbstractNodeVisitor
        {
        public:
            AbstractNodeVisitor(ParserBase* parser, bool nofollow = false);
            AbstractNodeVisitor(AbstractNodeVisitor* parent, bool nofollow = false);
            virtual ~AbstractNodeVisitor();

            virtual void init();

            ParserBase* parser() const;
            AbstractNodeVisitor* parent() const;

            #define CAT(a, b) a ## b
            #define DEF_NODE(name, ...) \
                virtual void visit(CAT(name, Node)* node) = 0;
            #define DEF_FLAG(name, value)
            #include "lang/ast/nodes.def"
            #undef DEF_FLAG
            #undef DEF_NODE
            #undef CAT

        protected:
            void M_message(Node* node, std::string const& msg);
            void M_error(Node* node, std::string const& msg);
            void M_warning(Node* node, std::string const& msg);
            virtual void M_follow(Node* node);

        private:
            ParserBase* m_parser;
            AbstractNodeVisitor* m_parent;
            bool m_nofollow;
        };

        class NodeVisitor : public AbstractNodeVisitor
        {
        public:
            using AbstractNodeVisitor::AbstractNodeVisitor;
            virtual ~NodeVisitor();

            virtual void init();
            virtual void visitDefault(Node* node);

            #define CAT(a, b) a ## b
            #define DEF_NODE(name, ...) \
                virtual void visit(CAT(name, Node)* node);
            #define DEF_FLAG(name, value)
            #include "lang/ast/nodes.def"
            #undef DEF_NODE
            #undef DEF_FLAG
            #undef CAT

        public:
            template <typename Generator, typename... Args>
            static void apply(Node* node, Args&&... args);
        };
    }
}

#endif // __AXOLOTL_LANG_AST_NODE_VISITOR_H__
