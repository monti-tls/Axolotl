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

#ifndef __AXOLOTL_BITS_BUFFER_H__
#define __AXOLOTL_BITS_BUFFER_H__

#include <cstddef>
#include <cstdint>

namespace bits
{
    //! This class is an abstract buffer interface
    //!   used to read or create a buffer.
    class Buffer
    {
    public:
        virtual ~Buffer() {}

        //! Create a clone of this buffer
        //! \return A clone of the buffer object
        virtual Buffer* copy() const = 0;

        //! Is this buffer read only ?
        //! \return true if the buffer is read-only, fale otherwise
        virtual bool readonly() const = 0;
        
        //! Get the total size of the buffer
        //! \return The size of the buffer in bytes
        virtual std::size_t size() const = 0;
        
        //! Access a byte located at a given position (const version)
        //! \param i The offset
        //! \return The read byte
        virtual uint8_t at(std::size_t i) const = 0;
        
        //! Access a byte located at a given position
        //! \param i The offset
        //! \return The read byte
        virtual uint8_t& at(std::size_t i) = 0;
        
        //! Insert a portion of `length' bytes at position `pos' in the
        //!   buffer, initialize the portion to 0
        //! \param pos The position at which the new portion is inserted
        //! \param length The length of the fragment to insert
        //! \return true if success, false otherwise
        virtual bool inject(std::size_t pos, std::size_t length) = 0;
        
        //! Copy data of length `length' from a buffer at position `pos'
        //! \param pos The position at which the copy starts
        //! \param data The buffer to copy
        //! \param length Length in bytes of the portion of the input buffer to copy
        //! \return true if success, false otherwise
        virtual bool copy(std::size_t pos, uint8_t* data, std::size_t length) = 0;
        
        //! Get a contiguous buffer to a portion of this buffer
        //! Use this with parcimony as it will enforce a join in chkunk list-based buffers
        //! The returned buffer will no longer be valid after deletion of this object
        //! \param pos The start position of the raw buffer to get
        //! \param len Length of the buffer to extract
        //! \return The extracted buffer if success, 0 otherwise
        virtual uint8_t* raw(std::size_t pos, std::size_t len) = 0;
        
        //! Get a sub-buffer starting at positon `pos' and of size `len' bytes
        //! The returned buffer will no longer be valid after deletion of this object
        //! \param pos The start position of the raw buffer to get
        //! \param len Length of the buffer to extract
        //! \return The extracted buffer if success, 0 otherwise
        virtual Buffer* sub(std::size_t pos, std::size_t len) = 0;

        int refcount;
    };
}

#endif // __AXOLOTL_BITS_BUFFER_H__
