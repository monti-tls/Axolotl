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

#ifndef __AXOLOTL_LANG_XLTL_SYMTAB_H__
#define __AXOLOTL_LANG_XLTL_SYMTAB_H__

#include "lang/xltl_symbol.hpp"
#include "lang/forward.hpp"

#include <string>
#include <list>

namespace lang
{
    class XltlSymtab
    {
    public:
        typedef std::list<XltlSymbol>::iterator iterator;
        typedef std::list<XltlSymbol>::const_iterator const_iterator;

        struct FindResult
        {
            XltlSymbol* symbol = nullptr;
            std::size_t index = 0UL;
            std::size_t args_count = 0UL;
            std::size_t locality = 0UL;
        };

    public:
        XltlSymtab(XltlSymtab* up = nullptr);
        ~XltlSymtab();

        XltlSymtab* up() const;

        bool add(XltlSymbol const& symbol, bool overwrite = false);
        bool find(std::string const& name, FindResult* res = nullptr) const;

        const_iterator begin() const;
        const_iterator end() const;

    private:
        const_iterator M_find(std::string const& name, FindResult* res = nullptr) const;

    private:
        XltlSymtab* m_up;
        std::list<XltlSymbol> m_symbols;
    };
}

#endif // __AXOLOTL_LANG_XLTL_SYMTAB_H__
