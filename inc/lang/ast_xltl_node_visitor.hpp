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

#ifndef __AXOLOTL_LANG_AST_XLTL_NODE_VISITOR_H__
#define __AXOLOTL_LANG_AST_XLTL_NODE_VISITOR_H__

#include "core/core.hpp"
#include "lang/forward.hpp"

#include <vector>
#include <string>

namespace lang
{
    namespace ast
    {
        class AbstractXltlNodeVisitor
        {
        public:
            AbstractXltlNodeVisitor(Parser* parser, bool nofollow = false);
            AbstractXltlNodeVisitor(XltlNodeVisitor* parent, bool nofollow = false);
            virtual ~AbstractXltlNodeVisitor();

            virtual void init();

            #define CAT(a, b, c) a ## b ## c
            #define DEF_NODE(name, ...) \
                virtual void visit(CAT(Xltl, name, Node)* node) = 0;
            #define DEF_FLAG(name, value)
            #include "lang/ast_xltl_nodes.def"
            #undef DEF_FLAG
            #undef DEF_NODE
            #undef CAT

        protected:
            void M_message(XltlNode* node, std::string const& msg);
            void M_error(XltlNode* node, std::string const& msg);
            void M_warning(XltlNode* node, std::string const& msg);
            virtual void M_follow(XltlNode* node);

        private:
            Parser* m_parser;
            bool m_nofollow;
        };

        class XltlNodeVisitor : public AbstractXltlNodeVisitor
        {
        public:
            using AbstractXltlNodeVisitor::AbstractXltlNodeVisitor;
            virtual ~XltlNodeVisitor();

            virtual void init();
            virtual void visitDefault(XltlNode* node);

            #define CAT(a, b, c) a ## b ## c
            #define DEF_NODE(name, ...) \
                virtual void visit(CAT(Xltl, name, Node)* node);
            #define DEF_FLAG(name, value)
            #include "lang/ast_xltl_nodes.def"
            #undef DEF_NODE
            #undef DEF_FLAG
            #undef CAT

        public:
            template <typename Generator, typename... Args>
            static void apply(XltlNode* node, Args&&... args);
        };
    }
}

#endif // __AXOLOTL_LANG_AST_XLTL_NODE_VISITOR_H__
