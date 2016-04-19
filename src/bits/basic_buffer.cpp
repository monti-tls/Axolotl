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

#include "bits/basic_buffer.hpp"

#include <cstring>

using namespace bits;

BasicBuffer::BasicBuffer()
    : m_owner(true)
    , m_raw(nullptr)
    , m_size(0)
{}

BasicBuffer::BasicBuffer(uint8_t* raw, std::size_t size)
    : m_owner(false)
    , m_raw(raw)
    , m_size(size)
{}

BasicBuffer::~BasicBuffer()
{
    if (m_owner && m_raw)
        delete[] m_raw;
}

Buffer* BasicBuffer::copy() const
{
    BasicBuffer* cpy = new BasicBuffer();

    if (m_size)
    {
        cpy->m_raw = new uint8_t[m_size];
        std::memcpy(cpy->m_raw, m_raw, m_size);    
    }
    
    return cpy;
}

bool BasicBuffer::readonly() const
{
    return !m_owner;
}

std::size_t BasicBuffer::size() const
{
    return m_size;
}

uint8_t BasicBuffer::at(std::size_t i) const
{
    return m_raw[i];
}

uint8_t& BasicBuffer::at(std::size_t i)
{
    return m_raw[i];
}

bool BasicBuffer::inject(std::size_t pos, std::size_t length)
{
    if (readonly())
        return false;

    uint8_t* raw = new uint8_t[m_size + length];
    if (!raw)
        return false;

    std::memcpy(raw, m_raw, pos);
    std::memset(raw + pos, 0, length);
    std::memcpy(raw + pos + length, m_raw + pos, m_size - pos);

    if (m_raw)
        delete[] m_raw;
    m_size += length;
    m_raw = raw;

    return true;
}

bool BasicBuffer::copy(std::size_t pos, uint8_t* data, std::size_t length)
{
    std::memcpy(m_raw + pos, data, length);
    return true;
}

uint8_t* BasicBuffer::raw(std::size_t pos, std::size_t len)
{
    if (pos + len > m_size)
        return nullptr;

    return m_raw + pos;
}

Buffer* BasicBuffer::sub(std::size_t pos, std::size_t len)
{
    if (pos + len > m_size)
        return nullptr;

    BasicBuffer* buf = new BasicBuffer();
    buf->m_owner = false;
    buf->m_raw = m_raw + pos;
    buf->m_size = len;
    return buf;
}
