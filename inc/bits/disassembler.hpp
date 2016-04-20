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

#ifndef __AXOLOTL_BITS_DISASSEMBLER_H__
#define __AXOLOTL_BITS_DISASSEMBLER_H__

#include "bits/blob.hpp"

#include <iostream>

namespace bits
{
    class Disassembler
    {
    public:
        Disassembler(Blob const& blob, std::ostream& os);

        void dumpAll();
        void dumpStrings();
        void dumpSymbols();
        void dumpTypeSpecs();
        void dumpConstants();
        void dumpText();

    private:
        void M_dumpSignature(blob_idx sigidx);

    private:
        Blob const& m_blob;
        std::ostream& m_os;
    };
}

#endif // __AXOLOTL_BITS_DISASSEMBLER_H__
