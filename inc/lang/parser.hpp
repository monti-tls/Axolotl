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

#ifndef __AXOLOTL_LANG_PARSER_H__
#define __AXOLOTL_LANG_PARSER_H__

#include "lang/forward.hpp"
#include "lang/parser_base.hpp"
#include "vm/module.hpp"

#include <string>
#include <iostream>
#include <map>
#include <stack>

namespace lang
{
    #define DEF_TOKEN(name, printable, prefer_lexeme) TOK_ ## name,
    #define DEF_ALIAS(name, alias) TOK_ ## name = TOK_ ## alias,
    enum
    {
        #include "lang/tokens.def"
    };
    #undef DEF_ALIAS
    #undef DEF_TOKEN

    class Parser : public ParserBase
    {
    public:
        Parser(std::istream& in, vm::Module const& module);
        ~Parser();

        ast::Node* parse();

    private:
        ast::Node* M_name();

        ast::Node* M_expr_0();
        ast::Node* M_expr_1();
        ast::Node* M_expr_2();
        ast::Node* M_expr_3();
        ast::Node* M_expr_4();
        ast::Node* M_expr_5();
        ast::Node* M_expr_6();
        ast::Node* M_expr_7();
        ast::Node* M_expr_8();
        ast::Node* M_expr_9();
        ast::Node* M_expr();
        ast::Node* M_expr_list();

        ast::Node* M_if_stmt();
        ast::Node* M_while_stmt();
        ast::Node* M_for_stmt();
        ast::Node* M_return_stmt();
        ast::Node* M_break_stmt();
        ast::Node* M_continue_stmt();
        ast::Node* M_block_stmt();

        void M_param_list_decl();
        ast::Node* M_fun_decl(bool in_class = false);

        ast::Node* M_class_decl();

        ast::Node* M_import_stmt();

        ast::Node* M_stmt();
        ast::Node* M_prog();

    private:
        virtual void M_setupLexer();
        virtual void M_setupTokens();
        ast::Node* M_check(ast::Node* node);
        Symtab* M_check(Symtab* symtab);
        
        std::string M_operatorMethodName(Token const& token);

        Symtab* M_scope() const;
        Symtab* M_pushScope();
        Symtab* M_popScope();

    private:
        std::stack<Symtab*> m_scope;
        vm::Module m_module;
    };
}

#endif // __AXOLOTL_LANG_PARSER_H__
