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

#ifndef __AXOLOTL_LANG_AST_NODE_H__
#define __AXOLOTL_LANG_AST_NODE_H__

#include "core/core.hpp"
#include "lang/forward.hpp"
#include "lang/token.hpp"
#include "lang/symtab.hpp"

#include <vector>
#include <string>

namespace lang
{
    namespace ast
    {
        //! The abstract base class for the Axolotl abstract
        //!   syntax tree and IR. This tree structure is versatile and
        //!   can represent either an AST or middle/low-level IRs.
        //! It consists of a basic tree structure where any node keeps
        //!   a list of its child nodes, and a pointer to it parent node.
        //! In order to extend the versatility of the representation, each node
        //!   can be linked to a next (and/or a previous) node, which is used
        //!   to build a doubly-linked list, called a chain of nodes.
        //! So this can be viewed as a tree of linked lists, which alleviates the need
        //!   to use special nodes to represent lists.
        class Node
        {
        public:
            //! Some flags used to categorize nodes
            enum Flags
            {
                #define DEF_NODE(name, ...)
                #define DEF_FLAG(name, value) name = value,
                #include "lang/ast/nodes.def"
                #undef DEF_FLAG
                #undef DEF_NODE
            };

        public:
            //! Build a node, remembering its position in the input
            //!   stream by saving its start token
            Node(Token const& start_token);
            virtual ~Node();

            //! Accept a visitor
            virtual void accept(AbstractNodeVisitor* v) = 0;
            //! Get this node's specific flags
            virtual Flags flags() const = 0;

            //! Get the start token for this node
            Token const& startToken() const;

            //! Get the children array of this node
            std::vector<Node*> const& siblings() const;
            //! Add a child to this node
            void addSibling(Node* sibling);

            //! Append a node to the chain in which the current
            //!   node resides
            void chain(Node* node);

            //! Get the previous node in the chain
            Node* prev() const;
            //! Get the next node in the chain
            Node* next() const;
            //! Get the last node in the chain
            Node* last() const;
            //! Get the length of the chain *starting at this node*
            std::size_t chainLength() const;
            //! Get the root of the current tree
            Node* root() const;

            //! Change the parent of this node and of all
            //!   subsequent nodes in the chain
            void setParent(Node* node);
            //! Get the parent of this node
            Node* parent() const;

            //! Attach a symbol table to this node
            void attachSymtab(Symtab* symtab);
            //! Detatch the symbol table from this node
            Symtab* detachSymtab();
            //! Get the nearest symbol table, either the one
            //!   attached to this node (if any), or parent()->symtab()
            Symtab* symtab() const;
            
            //! Substitute this node with another one, don't
            //!   preserve chaining
            void substituteWith(Node* node);
            //! Exchange this node with another one, keeping
            //!   the same children, keep in chain
            void exchangeWith(Node* node);
            //! Replace this node with another one, discarding
            //!   any children, keep in chain
            void replaceBy(Node* node);
            //! Remove this node from the chain
            void removeFromChain();
            //! Remove this node from the tree
            void remove();

        private:
            Node*& M_last();

        private:
            Token m_start_token;
            std::vector<Node*> m_siblings;
            Node* m_prev;
            Node* m_next;
            Node* m_parent;
            Symtab* m_symtab;
        };

        #define CAT(a, b) a ## b
        #define ATTR(type, name) type name;
        #define FLAGS(mask)
        #define DEF_NODE(name, ...) \
            class CAT(name, Node) : public Node \
            { \
            public: \
                using Node::Node; \
                virtual void accept(AbstractNodeVisitor* v); \
                virtual Flags flags() const; \
                __VA_ARGS__ \
            };
        #define DEF_FLAG(name, value)
        #include "lang/ast/nodes.def"
        #undef DEF_FLAG
        #undef DEF_NODE
        #undef FLAGS
        #undef ATTR
        #undef CAT
    }
}

#endif // __AXOLOTL_LANG_AST_NODE_H__
