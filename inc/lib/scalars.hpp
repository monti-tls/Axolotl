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

#ifndef __AXOLOTL_LIB_SCALARS_H__
#define __AXOLOTL_LIB_SCALARS_H__

#include "core/core.hpp"

#include <string>

namespace lib
{
    class Scalars
    {
    public:
        typedef bool Bool;
        typedef int Int;
        typedef char Char;
        typedef std::string String;
        typedef float Float;

    public:
        static void record();

        static void recordBool();
        static void recordInt();
        static void recordChar();
        static void recordString();
        static void recordFloat();
    };
}

#endif // __AXOLOTL_LIB_SCALARS_H__
