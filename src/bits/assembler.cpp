#include "bits/assembler.hpp"
#include "bits/basic_buffer.hpp"

#include <stdexcept>
#include <cstdlib>

using namespace bits;
using namespace core;

Operand::Operand()
    : m_which(Operand::None)
{}

Operand::Operand(std::string const& str, bool is_label)
    : m_which(is_label ? Operand::LabelRef : Operand::String)
    , m_data(str)
{}

Operand::Operand(int index)
    : m_which(Operand::Index)
    , m_data(index)
{}

Operand::~Operand()
{}

Operand::Which Operand::which() const
{ return m_which; }

Some const& Operand::data() const
{ return m_data; }

Operand Operand::label(std::string const& name)
{ return Operand(name, true); }

Assembler::Assembler(Blob& blob, std::size_t chunk_size)
    : m_blob(blob)
    , m_raw(nullptr)
    , m_raw_chunk_size(chunk_size)
    , m_raw_size(0)
    , m_raw_avail_size(0)
{}

Assembler::~Assembler()
{
    if (m_raw)
        finalize();
}

void Assembler::label(std::string const& name)
{
    m_labels[name] = pos();
}

void Assembler::emit(Opcode opcode, std::vector<Operand> const& operands)
{
    int nargs = opcode_nargs(opcode);
    if (nargs < 0)
        throw std::runtime_error("bits::Assembler::emit: invalid opcode");
    else if (nargs != (int) operands.size())
        throw std::runtime_error("bits::Assembler::emit: operand mismatch for " + opcode_as_string(opcode));

    M_write(opcode);

    for (auto op : operands)
    {
        switch (op.which())
        {
            case Operand::String:
            {
                blob_off sidx;
                if (!m_blob.addString(op.data().as<std::string>(), sidx))
                    throw std::runtime_error("bits::Assembler::emit: can't add string in blob");
                M_write(sidx);
                break;
            }

            case Operand::Index:
                M_write(op.data().as<int>());
                break;

            case Operand::LabelRef:
                m_label_refs.push_back(std::make_pair(pos(), op.data().as<std::string>()));
                M_write(0);
                break;

            default:
                break;
        }
    }
}

void Assembler::finalize()
{
    if (!m_raw)
        return;
    
    for (auto ref : m_label_refs)
    {
        auto label = m_labels.find(ref.second);
        if (label == m_labels.end())
            throw std::runtime_error("bits::Assembler::finalize: unresolved label `" + ref.second + "'");

        m_raw[ref.first] = label->second;
    }

    Buffer* buffer = new BasicBuffer((uint8_t*) m_raw, m_raw_size * sizeof(uint32_t));
    m_blob.setText(buffer);
    delete buffer;

    std::free(m_raw);
    m_raw = nullptr;
}

std::size_t Assembler::pos() const
{ return m_raw_size; }

std::size_t Assembler::M_write(uint32_t word)
{
    if (m_raw_size >= m_raw_avail_size)
    {
        m_raw_avail_size += m_raw_chunk_size;
        m_raw = (uint32_t*) std::realloc(m_raw, m_raw_avail_size * sizeof(uint32_t));
    }

    m_raw[m_raw_size++] = word;

    return m_raw_size - 1;
}
