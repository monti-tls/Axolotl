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

#ifndef __AXOLOTL_LANG_FORWARD_H__
#define __AXOLOTL_LANG_FORWARD_H__

namespace lang
{
    namespace nfa
    {
        struct State;
        struct MatchState;
        class Fragment;
    }

    struct RegularDefinition;
    class RegularDefinitionCompiler;
    class Lexer;
    class Token;
    class Parser;
    class XltlParser;

    namespace ast
    {
        class XltlNode;
        class AbstractXltlNodeVisitor;
        class XltlNodeVisitor;
        class XltlNodeGenerator;

        #define CAT(a, b, c) a ## b ## c
        #define DEF_FLAG(name, value)
        #define DEF_NODE(name, ...) \
            class CAT(Xltl, name, Node);
        #include "lang/ast_xltl_nodes.def"
        #undef DEF_NODE
        #undef DEF_FLAG
        #undef CAT
    }

    class XltlSymbol;
    class XltlSymtab;
    class XltlCompiler;

    namespace pass
    {
        class ExprResultCheck;
        class BindNames;
        class ResolveNames;
        class ResolveConsts;
        class GenerateRValue;
        class GenerateLValue;
        class GenerateIR;
    }
}

#endif // __AXOLOTL_LANG_FORWARD_H__
