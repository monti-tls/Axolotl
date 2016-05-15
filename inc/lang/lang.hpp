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

#include "lang/forward.hpp"
#include "lang/std_names.hpp"
#include "lang/nfa_state.hpp"
#include "lang/nfa_fragment.hpp"
#include "lang/regular_definition.hpp"
#include "lang/regular_definition_compiler.hpp"
#include "lang/lexer.hpp"
#include "lang/parser.hpp"
#include "lang/xltl_parser.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/ast_xltl_node_visitor.hpp"
#include "lang/ast_xltl_node_generator.hpp"
#include "lang/xltl_symbol.hpp"
#include "lang/xltl_symtab.hpp"
#include "lang/xltl_compiler.hpp"
#include "lang/pass_expr_result_check.hpp"
#include "lang/pass_bind_names.hpp"
#include "lang/pass_resolve_names.hpp"
#include "lang/pass_resolve_consts.hpp"
#include "lang/pass_generate_rvalue.hpp"
#include "lang/pass_generate_lvalue.hpp"
#include "lang/pass_generate_ir.hpp"

#include "lang/ast_xltl_node_visitor_impl.hpp"
#include "lang/ast_xltl_node_generator_impl.hpp"
