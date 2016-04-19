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

#ifndef __AXOLOTL_BITS_BASIC_BUFFER_H__
#define __AXOLOTL_BITS_BASIC_BUFFER_H__

#include "bits/buffer.hpp"

namespace bits
{
    //! This class implements the abstract Buffer
    //!   interface using a single contiguous memory range
    //!   (like std::vector does)
    class BasicBuffer : public Buffer
    {
    public:
        BasicBuffer();
        BasicBuffer(uint8_t* raw, std::size_t size);
        ~BasicBuffer();

        Buffer* copy() const;
        bool readonly() const;
        std::size_t size() const;
        uint8_t at(std::size_t i) const;
        uint8_t& at(std::size_t i);
        bool inject(std::size_t pos, std::size_t length);
        bool copy(std::size_t pos, uint8_t* data, std::size_t length);
        uint8_t* raw(std::size_t pos, std::size_t len);
        Buffer* sub(std::size_t pos, std::size_t len);

    private:
        bool m_owner;
        uint8_t* m_raw;
        std::size_t m_size;
    };
}

#endif // __AXOLOTL_BITS_BASIC_BUFFER_H__
