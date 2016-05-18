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

#include "bits/blob.hpp"
#include "bits/basic_buffer.hpp"

using namespace bits;

Blob::Blob()
{
    m_buffer = new BasicBuffer();
    m_buffer->refcount = 1;
}

Blob::Blob(Buffer* buffer)
{
    m_buffer = buffer;
    if (m_buffer)
        ++m_buffer->refcount;
}

Blob::Blob(Blob const& cpy)
{
    m_buffer = cpy.m_buffer;
    if (m_buffer)
        ++m_buffer->refcount;
}

Blob::~Blob()
{
    if (m_buffer && !--m_buffer->refcount)
        delete m_buffer;
}

Blob& Blob::operator=(Blob const& cpy)
{
    if (m_buffer && !--m_buffer->refcount)
        delete m_buffer;

    m_buffer = cpy.m_buffer;
    if (m_buffer)
        ++m_buffer->refcount;

    return *this;
}

Blob Blob::copy() const
{
    return Blob{m_buffer->copy()};
}

bool Blob::setModuleName(std::string const& module_name)
{
    blob_idx sidx;
    if (!addString(module_name, sidx))
        return false;

    blob_hdr* header = M_header();
    if (!header)
        return false;

    header->h_module_name = sidx;
    return true;
}

std::string Blob::moduleName() const
{
    std::string module_name = "";

    blob_hdr* header = M_header();
    if (header)
        string(header->h_module_name, module_name);

    return module_name;
}

bool Blob::string(blob_off off, std::string& str) const
{
    blob_shdr* strings = M_findSectionHeader(BLOB_ST_STRINGS);

    if (!strings || off >= strings->sh_size)
        return false;

    std::shared_ptr<Buffer> data = M_sectionData(strings);

    std::size_t len = 1;
    for (int i = (int) off; i < (int) strings->sh_size; ++i, ++len)
        if (!data->at(i))
            break;

    str = (const char*) data->raw(off, len);
    return true;
}

std::string Blob::string(blob_off soff) const
{
    std::string str = "";
    string(soff, str);
    return str;
}

bool Blob::addString(std::string const& str, blob_off& off)
{
    // Get (or create if necessary) the STRINGS section
    blob_shdr* strings = M_findSectionHeader(BLOB_ST_STRINGS);
    if (!strings)
    {
        strings = M_createSectionHeader();
        if (!strings)
            return false;
        strings->sh_type = BLOB_ST_STRINGS;
    }

    // Search if the string already exists
    if (findString(str, &off))
        return true;

    // Get string as ASCII
    const char* as_ascii = str.c_str();
    std::size_t as_ascii_len = str.size() + 1;
    off = strings->sh_size;

    // Grow section
    std::shared_ptr<Buffer> data = M_growSection(strings, as_ascii_len);
    if (!data)
        return false;

    // Copy string entry
    if (!data->copy(0, (uint8_t*) as_ascii, as_ascii_len))
        return false;

    return true;
}

bool Blob::findString(std::string const& str, blob_off* soff)
{
    blob_shdr* strings = M_findSectionHeader(BLOB_ST_STRINGS);
    if (!strings)
        return false;

    std::shared_ptr<Buffer> data = M_sectionData(strings);
    if (!data)
        return false;

    for (std::size_t pos = 0; pos < strings->sh_size;)
    {
        std::size_t len = 1;
        for (int i = 0; i < (int) (strings->sh_size - pos); ++i, ++len)
            if (!data->at(pos + i))
                break;

        if (str == (const char*) data->raw(pos, len))
        {
            if (soff)
                *soff = (blob_off) pos;
            return true;
        }

        pos += len;
    }

    return false;
}

blob_symbol* Blob::symbol(blob_idx symidx) const
{
    blob_shdr* symbols = M_findSectionHeader(BLOB_ST_SYMBOLS);
    if (!symbols)
        return nullptr;

    if (symidx >= symbols->sh_size / sizeof(blob_symbol))
        return nullptr;

    std::shared_ptr<Buffer> data = M_sectionData(symbols);
    if (!data)
        return nullptr;

    return (blob_symbol*) data->raw(symidx * sizeof(blob_symbol), sizeof(blob_symbol));
}

blob_symbol* Blob::addSymbol(std::string const& name, blob_idx* symidx)
{
    // First of all, allocate the name entry
    blob_off name_off;
    if (!addString(name, name_off))
        return nullptr;

    // Get (or create if necessary) the SYMBOLS section
    blob_shdr* symbols = M_findSectionHeader(BLOB_ST_SYMBOLS);
    if (!symbols)
    {
        symbols = M_createSectionHeader();
        if (!symbols)
            return nullptr;
        symbols->sh_type = BLOB_ST_SYMBOLS;
    }

    if (symidx)
        *symidx = (int) (symbols->sh_size / sizeof(blob_symbol));

    std::shared_ptr<Buffer> data = M_growSection(symbols, sizeof(blob_symbol));
    if (!data)
        return nullptr;

    blob_symbol* sym = (blob_symbol*) data->raw(0, sizeof(blob_symbol));
    sym->s_name = name_off;
    sym->s_type = BLOB_SYMT_NULL;
    sym->s_bind = BLOB_SYMB_NULL;
    sym->s_addr = 0;
    sym->s_nlocals = 0;
    sym->s_signature = 0;

    return sym;
}

blob_typespec* Blob::typeSpec(blob_idx tsidx, blob_off* tsoff) const
{
    blob_shdr* tspecs = M_findSectionHeader(BLOB_ST_TSPECS);
    if (!tspecs)
        return nullptr;

    std::shared_ptr<Buffer> data = M_sectionData(tspecs);
    if (!data)
        return nullptr;

    blob_idx idx = 0;
    for (std::size_t pos = 0; pos < tspecs->sh_size; ++idx)
    {
        blob_typespec* tspec = (blob_typespec*) data->raw(pos, sizeof(blob_typespec));
        if (idx == tsidx)
        {
            if (tsoff)
                *tsoff = pos;
            return tspec;
        }

        pos += sizeof(blob_typespec) + tspec->ts_symbols * sizeof(blob_idx);
    }

    return nullptr;
}

std::size_t Blob::typeSpecCount() const
{
    blob_shdr* tspecs = M_findSectionHeader(BLOB_ST_TSPECS);
    if (!tspecs)
        return 0;

    std::shared_ptr<Buffer> data = M_sectionData(tspecs);
    if (!data)
        return 0;

    blob_idx idx = 0;
    for (std::size_t pos = 0; pos < tspecs->sh_size; ++idx)
    {
        blob_typespec* tspec = (blob_typespec*) data->raw(pos, sizeof(blob_typespec));
        pos += sizeof(blob_typespec) + tspec->ts_symbols * sizeof(blob_idx);
    }

    return idx;
}

blob_typespec* Blob::addTypeSpec(std::string const& name, blob_idx* tsidx)
{
    // First of all, allocate the name entry
    blob_off name_off;
    if (!addString(name, name_off))
        return nullptr;

    // Get (or create if necessary) the TSPECS section
    blob_shdr* tspecs = M_findSectionHeader(BLOB_ST_TSPECS);
    if (!tspecs)
    {
        tspecs = M_createSectionHeader();
        if (!tspecs)
            return nullptr;
        tspecs->sh_type = BLOB_ST_TSPECS;
    }

    if (tsidx)
        *tsidx = typeSpecCount();

    std::shared_ptr<Buffer> data = M_growSection(tspecs, sizeof(blob_typespec));
    if (!data)
        return nullptr;

    blob_typespec* tspec = (blob_typespec*) data->raw(0, sizeof(blob_typespec));
    tspec->ts_name = name_off;
    tspec->ts_symbols = 0;

    return tspec;
}

bool Blob::addTypeSpecSymbol(blob_idx tsidx, blob_idx symidx)
{
    // Get the TSPECS section
    blob_shdr* tspecs = M_findSectionHeader(BLOB_ST_TSPECS);
    if (!tspecs)
        return false;

    blob_off tsoff;
    blob_typespec* tspec = typeSpec(tsidx, &tsoff);
    if (!tspec)
        return false;

    blob_off doff = tsoff + sizeof(blob_typespec) + tspec->ts_symbols * sizeof(blob_idx);
    ++tspec->ts_symbols;

    std::shared_ptr<Buffer> data = M_growSection(tspecs, sizeof(blob_idx), doff);
    if (!data)
        return false;

    if (!data->copy(0, (uint8_t*) &symidx, sizeof(blob_idx)))
        return false;

    return true;
}

blob_signature* Blob::signature(blob_idx sigidx, blob_off* sigoff) const
{
    blob_shdr* signatures = M_findSectionHeader(BLOB_ST_SIGNATURES);
    if (!signatures)
        return nullptr;

    std::shared_ptr<Buffer> data = M_sectionData(signatures);
    if (!data)
        return nullptr;

    blob_idx idx = 0;
    for (std::size_t pos = 0; pos < signatures->sh_size; ++idx)
    {
        blob_signature* sig = (blob_signature*) data->raw(pos, sizeof(blob_signature));
        if (idx == sigidx)
        {
            if (sigoff)
                *sigoff = pos;
            return sig;
        }

        pos += sizeof(blob_signature) + sig->si_argc * sizeof(blob_long);
    }

    return nullptr;
}

std::size_t Blob::signatureCount() const
{
    blob_shdr* signatures = M_findSectionHeader(BLOB_ST_SIGNATURES);
    if (!signatures)
        return 0;

    std::shared_ptr<Buffer> data = M_sectionData(signatures);
    if (!data)
        return 0;

    blob_idx idx = 0;
    for (std::size_t pos = 0; pos < signatures->sh_size; ++idx)
    {
        blob_signature* sig = (blob_signature*) data->raw(pos, sizeof(blob_signature));
        pos += sizeof(blob_signature) + sig->si_argc * sizeof(blob_long);
    }

    return idx;
}

bool Blob::addSignature(blob_idx& sigidx)
{
    // Get (or create if necessary) the SIGNATURES section
    blob_shdr* signatures = M_findSectionHeader(BLOB_ST_SIGNATURES);
    if (!signatures)
    {
        signatures = M_createSectionHeader();
        if (!signatures)
            return false;
        signatures->sh_type = BLOB_ST_SIGNATURES;
    }

    sigidx = signatureCount();

    std::shared_ptr<Buffer> data = M_growSection(signatures, sizeof(blob_signature));
    if (!data)
        return false;

    blob_signature* sig = (blob_signature*) data->raw(0, sizeof(blob_signature));
    sig->si_argc = 0;

    return true;
}

bool Blob::addSignatureArgument(blob_idx sigidx, blob_long classid)
{
    // Get the SIGNATURES section
    blob_shdr* signatures = M_findSectionHeader(BLOB_ST_SIGNATURES);
    if (!signatures)
        return false;

    blob_off sigoff;
    blob_signature* sig = signature(sigidx, &sigoff);
    if (!sig)
        return false;

    blob_off doff = sigoff + sizeof(blob_signature) + sig->si_argc * sizeof(blob_long);
    ++sig->si_argc;

    std::shared_ptr<Buffer> data = M_growSection(signatures, sizeof(blob_long), doff);
    if (!data)
        return false;

    if (!data->copy(0, (uint8_t*) &classid, sizeof(blob_long)))
        return false;

    return true;
}

blob_constant* Blob::constant(blob_idx cstidx) const
{
    blob_shdr* constants = M_findSectionHeader(BLOB_ST_CONSTANTS);
    if (!constants)
        return nullptr;

    if (cstidx >= constants->sh_size / sizeof(blob_constant))
        return nullptr;

    std::shared_ptr<Buffer> data = M_sectionData(constants);
    if (!data)
        return nullptr;

    return (blob_constant*) data->raw(cstidx * sizeof(blob_constant), sizeof(blob_constant));
}

blob_constant* Blob::addConstant(std::string const& type, std::string const& serialized, blob_idx* cstidx)
{
    // First of all, allocate the string entries
    blob_off type_off;
    if (!addString(type, type_off))
        return nullptr;

    blob_off serialized_off;
    if (!addString(serialized, serialized_off))
        return nullptr;

    blob_constant* already_there = nullptr;
    foreachConstant([&](blob_idx idx, blob_constant* cst)
    {
        if (cst->c_type == type_off && cst->c_serialized == serialized_off)
        {
            already_there = cst;
            if (cstidx)
                *cstidx = idx;
        }
    });

    if (already_there)
        return already_there;

    // Get (or create if necessary) the SYMBOLS section
    blob_shdr* constants = M_findSectionHeader(BLOB_ST_CONSTANTS);
    if (!constants)
    {
        constants = M_createSectionHeader();
        if (!constants)
            return nullptr;
        constants->sh_type = BLOB_ST_CONSTANTS;
    }

    if (cstidx)
        *cstidx = (int) (constants->sh_size / sizeof(blob_constant));

    std::shared_ptr<Buffer> data = M_growSection(constants, sizeof(blob_constant));
    if (!data)
        return nullptr;

    blob_constant* cst = (blob_constant*) data->raw(0, sizeof(blob_constant));
    cst->c_type = type_off;
    cst->c_serialized = serialized_off;

    return cst;
}

bool Blob::setText(Buffer* buffer)
{
    if (!buffer)
        return false;

    // Create the TEXT section
    blob_shdr* text = M_findSectionHeader(BLOB_ST_TEXT);
    if (!text)
    {
        text = M_createSectionHeader();
        if (!text)
            return false;
        text->sh_type = BLOB_ST_TEXT;
    }
    else
        return false;

    std::shared_ptr<Buffer> data = M_growSection(text, buffer->size());
    if (!data)
        return false;

    for (int i = 0; i < (int) buffer->size(); ++i)
        data->at(i) = buffer->at(i);

    return true;
}

std::shared_ptr<Buffer> Blob::text() const
{
    blob_shdr* text = M_findSectionHeader(BLOB_ST_TEXT);
    if (!text)
        return nullptr;

    return std::shared_ptr<Buffer>(m_buffer->sub(text->sh_offset, text->sh_size));
}

blob_debug_header* Blob::setDebugHeader(std::string const& file)
{
    // Add the file name
    blob_idx file_sidx;
    if (!addString(file, file_sidx))
        return nullptr;

    // Create the DEBUG section
    blob_shdr* debug = M_findSectionHeader(BLOB_ST_DEBUG);
    if (!debug)
    {
        debug = M_createSectionHeader();
        if (!debug)
            return nullptr;
        debug->sh_type = BLOB_ST_DEBUG;
    }
    else
        return nullptr;

    std::shared_ptr<Buffer> data = M_growSection(debug, sizeof(blob_debug_header));
    if (!data)
        return nullptr;

    blob_debug_header* header = (blob_debug_header*) data->raw(0, sizeof(blob_debug_header));
    header->d_file = file_sidx;
    header->d_count = 0;
    return header;
}

blob_debug_header* Blob::debugHeader() const
{
    blob_shdr* debug = M_findSectionHeader(BLOB_ST_DEBUG);
    if (!debug)
        return nullptr;

    std::shared_ptr<Buffer> data = M_sectionData(debug);
    if (!data)
        return nullptr;

    return (blob_debug_header*) data->raw(0, sizeof(blob_debug_header));
}

blob_debug_entry* Blob::addDebugEntry(blob_off line, blob_off col, blob_len extent, blob_idx* deidx)
{
    blob_debug_entry* already_there = nullptr;
    foreachDebugEntry([&](blob_idx idx, blob_debug_entry* entry)
    {
        if (entry->de_line == line && entry->de_col == col)
        {
            already_there = entry;
            if (deidx)
                *deidx = idx;
        }
    });

    if (already_there)
        return already_there;

    blob_shdr* debug = M_findSectionHeader(BLOB_ST_DEBUG);
    if (!debug)
        return nullptr;

    blob_debug_header* header = debugHeader();
    if (!header)
        return nullptr;
    int idx = header->d_count++;

    if (deidx)
        *deidx = idx;

    std::shared_ptr<Buffer> data = M_growSection(debug, sizeof(blob_debug_entry));
    if (!data)
        return nullptr;

    blob_debug_entry* entry = (blob_debug_entry*) data->raw(0, sizeof(blob_debug_entry));
    entry->de_line = line;
    entry->de_col = col;
    entry->de_extent = extent;

    return entry;
}

blob_debug_entry* Blob::debugEntry(blob_idx deidx) const
{
    blob_shdr* debug = M_findSectionHeader(BLOB_ST_DEBUG);
    if (!debug)
        return nullptr;

    blob_debug_header* header = debugHeader();
    if (!header)
        return nullptr;

    if (deidx >= header->d_count)
        return nullptr;

    std::shared_ptr<Buffer> data = M_sectionData(debug);
    if (!data)
        return nullptr;

    return (blob_debug_entry*) data->raw(sizeof(blob_debug_header) + deidx * sizeof(blob_debug_entry), sizeof(blob_debug_entry));
}

void Blob::foreachString(std::function<void(blob_idx, std::string const&)> const& action) const
{
    blob_shdr* strings = M_findSectionHeader(BLOB_ST_STRINGS);
    if (!strings)
        return;

    std::shared_ptr<Buffer> data = M_sectionData(strings);
    if (!data)
        return;

    for (std::size_t pos = 0; pos < strings->sh_size;)
    {
        std::size_t len = 1;
        for (int i = 0; i < (int) (strings->sh_size - pos); ++i, ++len)
            if (!data->at(pos + i))
                break;

        std::string str = (const char*) data->raw(pos, len);
        action(pos, str);

        pos += len;
    }
}

void Blob::foreachSymbol(std::function<void(blob_idx, blob_symbol*)> const& action) const
{
    blob_shdr* symbols = M_findSectionHeader(BLOB_ST_SYMBOLS);
    if (!symbols)
        return;

    int count = (int) (symbols->sh_size / sizeof(blob_symbol));
    for (int i = 0; i < count; ++i)
        action(i, symbol(i));
}

void Blob::foreachTypeSpec(std::function<void(blob_idx, blob_typespec*)> const& action) const
{
    int count = (int) typeSpecCount();
    for (int i = 0; i < count; ++i)
        action(i, typeSpec(i));
}

bool Blob::foreachTypeSpecSymbol(blob_idx tsidx, std::function<void(blob_idx)> const& action) const
{
    // Get the TSPECS section
    blob_shdr* tspecs = M_findSectionHeader(BLOB_ST_TSPECS);
    if (!tspecs)
        return false;

    std::shared_ptr<Buffer> data = M_sectionData(tspecs);
    if (!data)
        return false;

    blob_off tsoff;
    blob_typespec* tspec = typeSpec(tsidx, &tsoff);
    if (!tspec)
        return false;

    for (int i = 0; i < (int) tspec->ts_symbols; ++i)
    {
        blob_off off = tsoff + sizeof(blob_typespec) + i * sizeof(blob_idx);
        action(*((blob_idx*) data->raw(off, sizeof(blob_idx))));
    }

    return true;
}

void Blob::foreachSignature(std::function<void(blob_idx, blob_signature*)> const& action) const
{
    int count = (int) signatureCount();
    for (int i = 0; i < count; ++i)
        action(i, signature(i));
}

bool Blob::foreachSignatureArgument(blob_idx sigidx, std::function<void(blob_long)> const& action) const
{
    // Get the SIGNATURES section
    blob_shdr* signatures = M_findSectionHeader(BLOB_ST_SIGNATURES);
    if (!signatures)
        return false;

    std::shared_ptr<Buffer> data = M_sectionData(signatures);
    if (!data)
        return false;

    blob_off sigoff;
    blob_signature* sig = signature(sigidx, &sigoff);
    if (!sig)
        return false;

    for (int i = 0; i < (int) sig->si_argc; ++i)
    {
        blob_off off = sigoff + sizeof(blob_signature) + i * sizeof(blob_long);
        action(*((blob_long*) data->raw(off, sizeof(blob_long))));
    }

    return true;
}

void Blob::foreachConstant(std::function<void(blob_idx, blob_constant*)> const& action) const
{
    blob_shdr* constants = M_findSectionHeader(BLOB_ST_CONSTANTS);
    if (!constants)
        return;

    int count = (int) (constants->sh_size / sizeof(blob_constant));
    for (int i = 0; i < count; ++i)
        action(i, constant(i));
}

void Blob::foreachDebugEntry(std::function<void(blob_idx, blob_debug_entry*)> const& action) const
{
    blob_shdr* debug = M_findSectionHeader(BLOB_ST_DEBUG);
    if (!debug)
        return;

    blob_debug_header* header = debugHeader();
    if (!header)
        return;

    for (int i = 0; i < (int) header->d_count; ++i)
        action(i, debugEntry(i));
}

blob_hdr* Blob::M_header() const
{
    return (blob_hdr*) m_buffer->raw(0, sizeof(blob_hdr));
}

blob_shdr* Blob::M_sectionHeader(blob_idx sidx) const
{
    blob_hdr* hdr = M_header();
    if (!hdr || sidx >= hdr->h_shnum)
        return nullptr;

    return (blob_shdr*) m_buffer->raw(hdr->h_shoff + sidx * sizeof(blob_shdr), sizeof(blob_shdr));
}

blob_idx Blob::M_sectionIndex(blob_shdr* shdr) const
{
    blob_hdr* hdr = M_header();
    if (!hdr || !shdr)
        return false;

    return (blob_idx) (shdr - (blob_shdr*) m_buffer->raw(hdr->h_shoff, sizeof(blob_shdr)));
}

std::shared_ptr<Buffer> Blob::M_sectionData(blob_idx sidx) const
{
    return M_sectionData(M_sectionHeader(sidx));
}

std::shared_ptr<Buffer> Blob::M_sectionData(blob_shdr* shdr) const
{
    if (!shdr || !shdr->sh_offset || !shdr->sh_size)
        return nullptr;

    return std::shared_ptr<Buffer>(m_buffer->sub(shdr->sh_offset, shdr->sh_size));
}

blob_idx Blob::M_findSectionIndex(blob_shtype type) const
{
    blob_hdr* hdr = M_header();
    if (!hdr)
        return 0;

    for (int i = 0; i < (int) hdr->h_shnum; ++i)
    {
        if (M_sectionHeader(i)->sh_type == type)
            return i;
    }

    return 0;
}

blob_shdr* Blob::M_findSectionHeader(blob_shtype type) const
{
    blob_idx idx = M_findSectionIndex(type);
    if (!idx)
        return nullptr;

    return M_sectionHeader(idx);
}

bool Blob::M_createHeader()
{
    if (m_buffer->size())
        return false;

    if (!m_buffer->inject(0, sizeof(blob_hdr)))
        return false;

    blob_hdr* hdr = M_header();
    if (!hdr)
        return false;

    hdr->h_magic = BLOB_MAGIC;
    hdr->h_version = BLOB_VERSION;
    hdr->h_shoff = 0;
    hdr->h_shnum = 0;

    return true;
}

blob_shdr* Blob::M_createSectionHeader(blob_idx* sidx)
{
    if (!m_buffer->size())
        M_createHeader();

    blob_hdr* hdr = M_header();
    if (!hdr)
        return nullptr;

    // If empty, create the first NULL section
    if (!hdr->h_shnum)
    {
        hdr->h_shnum = 1;
        hdr->h_shoff = sizeof(blob_hdr);

        blob_shdr shdr = { BLOB_ST_NULL, 0, 0 };
        if (!m_buffer->inject(sizeof(blob_hdr), sizeof(blob_shdr)))
            return nullptr;

        if (!m_buffer->copy(sizeof(blob_hdr), (uint8_t*) &shdr, sizeof(blob_shdr)))
            return nullptr;

        hdr = M_header();
        if (!hdr)
            return nullptr;
    }

    // Save the new section's index
    blob_idx new_sidx = hdr->h_shnum++;

    // We will insert the header at this position
    std::size_t inject_at = sizeof(blob_hdr) + new_sidx * sizeof(blob_shdr);

    // Shift any section which has data after the insertion point
    for (int i = 0; i < (int) new_sidx; ++i)
    {
        blob_shdr* shdr = M_sectionHeader(i);
        if (!shdr)
            return nullptr;

        if (shdr->sh_offset >= inject_at)
            shdr->sh_offset += sizeof(blob_shdr);
    }

    if (!m_buffer->inject(inject_at, sizeof(blob_shdr)))
        return nullptr;

    blob_shdr shdr = { BLOB_ST_NULL, 0, 0 };
    if (!m_buffer->copy(inject_at, (uint8_t*) &shdr, sizeof(blob_shdr)))
        return nullptr;

    if (sidx)
        *sidx = new_sidx;

    return M_sectionHeader(new_sidx);
}

std::shared_ptr<Buffer> Blob::M_growSection(blob_idx sidx, blob_len amount, long doff)
{
    return M_growSection(M_sectionHeader(sidx), amount, doff);
}

std::shared_ptr<Buffer> Blob::M_growSection(blob_shdr* shdr, blob_len amount, long doff)
{
    if (!shdr)
        return 0;

    blob_hdr* hdr = M_header();
    if (!hdr)
        return 0;

    // If the section is empty, make it start at the end of the buffer
    if (!shdr->sh_offset)
        shdr->sh_offset = m_buffer->size();

    // We will insert the new data at this position in the buffer
    std::size_t inject_at = shdr->sh_offset + (doff >= 0 ? doff : shdr->sh_size);

    // Shift any section which has data after the insertion point
    for (int i = 0; i < (int) hdr->h_shnum; ++i)
    {
        blob_shdr* shdr = M_sectionHeader(i);
        if (!shdr)
            return nullptr;
        if (shdr->sh_offset >= inject_at && shdr->sh_size)
            shdr->sh_offset += amount;
    }

    shdr->sh_size += amount;

    if (!m_buffer->inject(inject_at, amount))
        return nullptr;

    return std::shared_ptr<Buffer>(m_buffer->sub(inject_at, amount));
}
