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

#ifndef __AXOLOTL_BITS_ASSEMBLER_H__
#define __AXOLOTL_BITS_ASSEMBLER_H__

#include "bits/blob.hpp"
#include "bits/opcodes.hpp"
#include "core/some.hpp"
#include "lang/forward.hpp"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace bits
{
    class Operand
    {
    public:
        enum Which
        {
            None,
            String,
            Index,
            LabelRef
        };

    public:
        Operand();
        Operand(std::string const& str, bool is_label = false);
        Operand(int index);
        ~Operand();

        Which which() const;
        core::Some const& data() const;

        static Operand label(std::string const& name);

    private:
        Which m_which;
        core::Some m_data;
    };

    class DebugInfo
    {
    public:
        DebugInfo();
        DebugInfo(std::size_t line, std::size_t col, std::size_t extent = 0);
        DebugInfo(lang::Token const& token);
        ~DebugInfo();

        bool empty() const;
        std::size_t line() const;
        std::size_t col() const;
        std::size_t extent() const;

    private:
        bool m_empty;
        std::size_t m_line;
        std::size_t m_col;
        std::size_t m_extent;
    };

    class Assembler
    {
    public:
        Assembler(Blob& blob, std::size_t chunk_size = 32);
        ~Assembler();

        void setDebugInfo(std::string const& file);
        void label(std::string const& name);
        void emit(Opcode opcode, std::vector<Operand> const& operands, DebugInfo const& debug = DebugInfo());
        void finalize();

        std::size_t pos() const;

    private:
        std::size_t M_write(uint32_t word);

    private:
        Blob& m_blob;

        uint32_t* m_raw;
        std::size_t m_raw_chunk_size;
        std::size_t m_raw_size;
        std::size_t m_raw_avail_size;

        std::map<std::string, std::size_t> m_labels;
        std::list<std::pair<std::size_t, std::string>> m_label_refs;
    };
}

#endif // __AXOLOTL_BITS_ASSEMBLER_H__
