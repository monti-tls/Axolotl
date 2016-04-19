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

#ifndef __AXOLOTL_BITS_BLOB_H__
#define __AXOLOTL_BITS_BLOB_H__

#include "bits/buffer.hpp"

#include <string>
#include <functional>
#include <memory>
#include <cstdint>

namespace bits
{
    //! This is 'AXOL' in ASCII
    static constexpr uint32_t BLOB_MAGIC = 0x4C4F5841;

    //! Version 0.1.1
    static constexpr uint32_t BLOB_VERSION = 0x00010001;

    //! Offset type
    typedef uint32_t blob_off;

    //! Length type
    typedef uint32_t blob_len;

    //! Index type (to identify sections and symbols)
    typedef uint32_t blob_idx;

    //! Holds possible values for blob_shdr->sh_type
    enum blob_shtype
    {
        BLOB_ST_NULL = 0x00,
        BLOB_ST_STRINGS,
        BLOB_ST_TEXT,
        BLOB_ST_SYMBOLS,
        BLOB_ST_TSPECS,
        BLOB_ST_SIGNATURES,
        BLOB_ST_CONSTANTS
    };

    //! Holds possible values for blob_symbol->s_type
    enum blob_symtype
    {
        BLOB_SYMT_NULL = 0x00,
        BLOB_SYMT_FUNCTION,
        BLOB_SYMT_METHOD
    };

    //! Holds possible values for blob_symbol->s_bind
    enum blob_symbind
    {
        BLOB_SYMB_NULL = 0x00,
        BLOB_SYMB_LOCAL = 0x01,
        BLOB_SYMB_GLOBAL = 0x02
    };

    //! Header of the code image
    struct __attribute__((packed)) blob_hdr
    {
        //! Magic number, should be IMAGE_MAGIC
        uint32_t h_magic;
        //! Version number, should be IMAGE_VERSION
        uint32_t h_version;
        //! Offset in bytes to the section header table
        blob_off h_shoff;
        //! Number of section headers
        blob_len h_shnum;
    };

    //! A section header
    struct __attribute__((packed)) blob_shdr
    {
        //! Type of the section
        blob_shtype sh_type;
        //! Offset in bytes to the beginning of the section's
        //!   data
        blob_off sh_offset;
        //! Length in bytes of the section data
        blob_len sh_size;
    };

    //! Symbol list entry for a type specification
    struct __attribute__((packed)) blob_symbol
    {
        //! Name of the symbol entry (as an offset in the string table)
        blob_off s_name;
        //! Type of the symbol
        blob_symtype s_type;
        //! Binding of the symbol
        blob_symbind s_bind;
        //! Address of the symbol (as an offset in the text section)
        blob_off s_addr;
        //! Number of locals in the function
        blob_len s_nlocals;
        //! Index of the signature entry for this symbol
        blob_off s_signature;
    };

    //! Type specification entry header
    struct __attribute__((packed)) blob_typespec
    {
        //! Name of the type entry (as an offset in the string table)
        blob_off ts_name;
        //! Number of symbols in the the symbol list (each entry is a blob_idx)
        blob_len ts_symbols;
    };

    //! Signature entry header
    struct __attribute((packed)) blob_signature
    {
        //! Number of arguments in the type list (each entry is a blob_off
        //!   pointing to the type name in the string table)
        blob_len si_argc;
    };

    //! Constant entry
    struct __attribute__((packed)) blob_constant
    {
        //! Type name as a string table entry offset
        blob_off c_type;
        //! Serialized data as a string table entry offset
        blob_off c_serialized;
    };

    //! The Blob class represents a binary blob and provides
    //!   read and write primitives to the underlying buffer / file.
    class Blob
    {
    public:
        //! Create a new buffer for edition.
        Blob();
        
        //! Copy constructor, as this object is reference-counter
        //!   the copied instance will modify the same underlying buffer.
        Blob(Blob const& cpy);
        
        //! Create a blob using a predefined buffer.
        Blob(Buffer* buffer);

        ~Blob();

        //! See the copy constructor.
        Blob& operator=(Blob const& cpy);

        //! Create a clone of the blob that manipulates
        //!   a different buffer.
        Blob copy() const;

        //! Get the string at offset `soff' in the string table.
        //! \param soff Offset of the string in the string table
        //! \param str Output parameter for the result string
        //! \return true if successful, false otherwise
        bool string(blob_off soff, std::string& str) const;

        //! Get the string associated with offet `soff' if it exists,
        //!   return an empty string otherwise.
        //! \param soff Offset of the string in the string table
        //! \return The associated string if success, an empty string otherwise
        std::string string(blob_off soff) const;

        //! Add a new string in the string table. If the string exists,
        //!   it is not added twice to avoid duplicates.
        //! \param str The string to add into the string table
        //! \param sidx Output parameter for the added string index
        //! \return true if successful, false otherwise
        bool addString(std::string const& str, blob_off& sidx);

        //! Find a string in the string table.
        //! \param str The string to find in the string table
        //! \param soff Optional output parameter for the
        //!             eventually found string offset
        //! \return true if found, false otherwise
        bool findString(std::string const& str, blob_off* soff = nullptr);

        //! Get the symbol entry associated with index `symidx'
        //! \param symidx The symbol index
        //! \return The symbol if success, 0 otherwise
        blob_symbol* symbol(blob_idx symidx) const;

        //! Add a symbol entry in the symbol table
        //! \param name The name of the symbol to add
        //! \param symidx Optional output parameter for the index of the added symbol
        //! \return The added symbol if success, 0 otherwise
        blob_symbol* addSymbol(std::string const& name, blob_idx* symidx = nullptr);

        //! Get the type specification entry associated with index `tsidx'
        //! \param tsidx The index of the entry to get
        //! \param tsoff Optional output parameter for the offset of the entry within
        //!              the type specifications section (as entries are of variable size)
        //! \return The added type specification entry
        blob_typespec* typeSpec(blob_idx tsidx, blob_off* tsoff = nullptr) const;

        //! Get the number of type specification entries.
        //! \return The number of typespecs entries.
        std::size_t typeSpecCount() const;

        //! Add a new type specification to the blob.
        //! \param name Name of the type specification to add
        //! \param tsidx Optional output parameter for the index of the added entry
        //! \return The added entry if success, 0 otherwise
        blob_typespec* addTypeSpec(std::string const& name, blob_idx* tsidx = nullptr);

        //! Add a new symbol to a type specification entry.
        //! \param tsidx The index of the type specification to extend with the new symbol
        //! \param symidx Index of the symbol to add to the type specification
        //! \return true if success, false otherwise
        bool addTypeSpecSymbol(blob_idx tsidx, blob_idx symidx);

        blob_signature* signature(blob_idx sigidx, blob_off* sigoff = nullptr) const;
        std::size_t signatureCount() const;
        bool addSignature(blob_idx& sigidx);
        bool addSignatureArgument(blob_idx sigidx, std::string const& type);

        blob_constant* constant(blob_idx cstidx) const;
        blob_constant* addConstant(std::string const& type, std::string const& serialized, blob_idx* cstidx = nullptr);

        //! Sets the TEXT section contents wich the given buffer
        //! \param buffer The buffer to fill the TEXT section with
        //! \return true if success, false otherwise
        bool setText(Buffer* buffer);

        //! Get a buffer to the TEXT section's contents
        //! \return A buffer pointing to the TEXT section's contents if success, 0 otherwise
        std::shared_ptr<Buffer> text() const;

        void foreachString(std::function<void(blob_idx, std::string const&)> const& action) const;

        //! Loop over every symbol entry and execute `action'
        //! \param action The action to execute on every symbol
        void foreachSymbol(std::function<void(blob_idx, blob_symbol*)> const& action) const;

        //! Loop over every type specification and execute `action'
        //! \param action The action to execute on every type specification
        void foreachTypeSpec(std::function<void(blob_idx, blob_typespec*)> const& action) const;

        //! Loop over every symbol in a given type specification
        //! \param tsidx The index of the type specification to iterate on
        //! \param action The action to execute on every symbol registered within
        //!               the type specification of index `tsidx'
        //! \return true if success, false otherwise
        bool foreachTypeSpecSymbol(blob_idx tsidx, std::function<void(blob_idx)> const& action) const;

        void foreachSignature(std::function<void(blob_idx, blob_signature*)> const& action) const;
        bool foreachSignatureArgument(blob_idx sigidx, std::function<void(blob_off)> const& action) const;

        void foreachConstant(std::function<void(blob_idx, blob_constant*)> const& action) const;

     private:
        blob_hdr* M_header() const;
        blob_shdr* M_sectionHeader(blob_idx sidx) const;
        blob_idx M_sectionIndex(blob_shdr* shdr) const;
        std::shared_ptr<Buffer> M_sectionData(blob_idx sidx) const;
        std::shared_ptr<Buffer> M_sectionData(blob_shdr* shdr) const;
        blob_idx M_findSectionIndex(blob_shtype type) const;
        blob_shdr* M_findSectionHeader(blob_shtype type) const;

        bool M_createHeader();
        blob_shdr* M_createSectionHeader(blob_idx* sidx = nullptr);
        std::shared_ptr<Buffer> M_growSection(blob_idx sidx, blob_len amount, long split = -1);
        std::shared_ptr<Buffer> M_growSection(blob_shdr* shdr, blob_len amount, long split = -1);

     private:
        Buffer* m_buffer;
    };
}

#endif // __AXOLOTL_BITS_BLOB_H__
