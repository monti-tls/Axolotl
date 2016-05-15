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

#ifndef __AXOLOTL_LANG_XLTL_PARSER_H__
#define __AXOLOTL_LANG_XLTL_PARSER_H__

#include "lang/forward.hpp"
#include "lang/parser.hpp"

#include <string>
#include <iostream>
#include <map>
#include <stack>

namespace lang
{
    #define DEF_TOKEN(name) TOK_ ## name,
    #define DEF_ALIAS(name, alias) TOK_ ## name = TOK_ ## alias,
    enum
    {
        #include "lang/xltl_tokens.def"
    };
    #undef DEF_ALIAS
    #undef DEF_TOKEN

    class XltlParser : public Parser
    {
    public:
        XltlParser(std::istream& in);
        ~XltlParser();

        ast::XltlNode* parse();

    private:
        ast::XltlNode* M_name();

        ast::XltlNode* M_expr_0();
        ast::XltlNode* M_expr_1();
        ast::XltlNode* M_expr_2();
        ast::XltlNode* M_expr_3();
        ast::XltlNode* M_expr_4();
        ast::XltlNode* M_expr_5();
        ast::XltlNode* M_expr_6();
        ast::XltlNode* M_expr_7();
        ast::XltlNode* M_expr_8();
        ast::XltlNode* M_expr_9();
        ast::XltlNode* M_expr();
        ast::XltlNode* M_expr_list();

        ast::XltlNode* M_if_stmt();
        ast::XltlNode* M_while_stmt();
        ast::XltlNode* M_return_stmt();
        ast::XltlNode* M_block_stmt();

        void M_param_list_decl();
        ast::XltlNode* M_fun_decl();

        ast::XltlNode* M_stmt();
        ast::XltlNode* M_prog();

    private:
        virtual void M_setupLexer();
        virtual void M_setupTokens();
        ast::XltlNode* M_check(ast::XltlNode* node);
        XltlSymtab* M_check(XltlSymtab* symtab);
        
        std::string M_operatorMethodName(Token const& token);

        XltlSymtab* M_scope() const;
        XltlSymtab* M_pushScope();
        XltlSymtab* M_popScope();

    private:
        std::stack<XltlSymtab*> m_scope;
    };
}

#endif // __AXOLOTL_LANG_XLTL_PARSER_H__
