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
#include "lang/regular_definition.hpp"
#include "lang/regular_definition_compiler.hpp"
#include "lang/lexer.hpp"
#include "lang/parser_base.hpp"
#include "lang/parser.hpp"
#include "lang/symbol.hpp"
#include "lang/symtab.hpp"
#include "lang/compiler.hpp"

#include "lang/nfa/nfa.hpp"
#include "lang/ast/ast.hpp"
#include "lang/pass/pass.hpp"
