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

#include "lang/pass/forward.hpp"
#include "lang/pass/expr_result_check.hpp"
#include "lang/pass/bind_names.hpp"
#include "lang/pass/resolve_names.hpp"
#include "lang/pass/resolve_consts.hpp"
#include "lang/pass/generate_rvalue.hpp"
#include "lang/pass/generate_lvalue.hpp"
#include "lang/pass/generate_ir.hpp"
#include "lang/pass/pretty_print.hpp"
