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

#ifndef __AXOLOTL_LANG_STD_NAMES_H__
#define __AXOLOTL_LANG_STD_NAMES_H__

namespace lang
{
    // All names containing an `@' are strictly private
    //   and can't collide with user-defined symbols
    
    constexpr auto std_core_module_name = "core";

    constexpr auto std_callable_classname = "@callable";
    constexpr auto std_nil_classname = "@nil";
    constexpr auto std_const_dict  = "@consts";
    constexpr auto std_any_type    = "*";
    constexpr auto std_package_wildcard = "*";

    constexpr auto std_main        = "__main__";

    constexpr auto std_classname   = "__classname__";
    constexpr auto std_classid     = "__classid__";
    constexpr auto std_del         = "__del__";
    constexpr auto std_call        = "__call__";
    constexpr auto std_serialize   = "__serialize__";
    constexpr auto std_unserialize = "__unserialize__";

    constexpr auto std_add         = "__add__";
    constexpr auto std_sub         = "__sub__";
    constexpr auto std_neg         = "__neg__";
    constexpr auto std_mul         = "__mul__";
    constexpr auto std_div         = "__div__";
    constexpr auto std_mod         = "__mod__";
    constexpr auto std_and         = "__and__";
    constexpr auto std_or          = "__or__";
    constexpr auto std_not         = "__not__";
    constexpr auto std_equals      = "__equals__";
    constexpr auto std_nequals     = "__nequals__";
    constexpr auto std_lt          = "__lt__";
    constexpr auto std_lte         = "__lte__";
    constexpr auto std_gt          = "__gt__";
    constexpr auto std_gte         = "__gte__";

    constexpr auto std_self        = "self";
}

#endif // __AXOLOTL_LANG_STD_NAMES_H__
