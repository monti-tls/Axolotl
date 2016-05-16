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

#ifndef __AXOLOTL_LANG_SYMTAB_H__
#define __AXOLOTL_LANG_SYMTAB_H__

#include "lang/symbol.hpp"
#include "lang/forward.hpp"

#include <string>
#include <list>

namespace lang
{
    class Symtab
    {
    public:
        typedef std::list<Symbol>::iterator iterator;
        typedef std::list<Symbol>::const_iterator const_iterator;

        struct FindResult
        {
            Symbol* symbol = nullptr;
            std::size_t index = 0UL;
            std::size_t args_count = 0UL;
            std::size_t locality = 0UL;
        };

    public:
        Symtab(Symtab* up = nullptr);
        ~Symtab();

        Symtab* up() const;

        bool add(Symbol const& symbol, bool overwrite = false);
        bool find(std::string const& name, FindResult* res = nullptr) const;

        const_iterator begin() const;
        const_iterator end() const;

        std::size_t localsCount() const;
        std::size_t argumentsCount() const;

    private:
        const_iterator M_find(std::string const& name, FindResult* res = nullptr) const;

    private:
        Symtab* m_up;
        std::list<Symbol> m_symbols;
    };
}

#endif // __AXOLOTL_LANG_SYMTAB_H__
