#include "lang/xltl_symtab.hpp"
#include "lang/ast_xltl_node.hpp"

using namespace lang;
using namespace ast;

XltlSymtab::XltlSymtab(XltlSymtab* up)
    : m_up(up)
{}

XltlSymtab::~XltlSymtab()
{}

XltlSymtab* XltlSymtab::up() const
{ return m_up; }

bool XltlSymtab::add(XltlSymbol const& symbol, bool overwrite)
{
    if (!overwrite && M_find(symbol.name()) != m_symbols.end())
        return false;

    XltlSymbol copy = symbol;
    if (symbol.which() == XltlSymbol::Auto)
        copy.setBinding(m_up ? XltlSymbol::Local : XltlSymbol::Global);

    m_symbols.push_back(copy);

    return true;
}

bool XltlSymtab::find(std::string const& name, FindResult* res) const
{
    auto it = M_find(name, res);
    if (it != m_symbols.end())
        return true;

    if (res)
        ++res->locality;

    return m_up ? m_up->find(name, res) : false;
}

XltlSymtab::const_iterator XltlSymtab::begin() const
{ return m_symbols.begin(); }

XltlSymtab::const_iterator XltlSymtab::end() const
{ return m_symbols.end(); }

XltlSymtab::const_iterator XltlSymtab::M_find(std::string const& name, FindResult* res) const
{
    std::size_t index = 0;
    std::size_t args_count = 0;
    for (const_iterator it = m_symbols.begin(); it != m_symbols.end(); ++it, ++index)
    {
        if (it->which() == XltlSymbol::Argument)
            ++args_count;

        if (it->name() == name)
        {
            if (res)
            {
                res->symbol = const_cast<XltlSymbol*>(&*it);
                res->index = index;
                res->args_count = args_count;
            }
            return it;
        }
    }

    return m_symbols.end();
}
