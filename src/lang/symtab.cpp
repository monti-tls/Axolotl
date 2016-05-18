#include "lang/symtab.hpp"
#include "lang/ast/node.hpp"

using namespace lang;
using namespace ast;

Symtab::Symtab(Symtab* up)
    : m_up(up)
{}

Symtab::~Symtab()
{}

Symtab* Symtab::top() const
{
    if (m_up)
        return m_up->top();

    return const_cast<Symtab*>(this);
}

Symtab* Symtab::up() const
{ return m_up; }

bool Symtab::add(Symbol const& symbol, bool overwrite)
{
    if (symbol.which() == Symbol::Const)
    {
        if (m_up)
            return m_up->add(symbol, overwrite);
        else
            overwrite = true;
    }

    if (!overwrite && M_find(symbol.name()) != m_symbols.end())
        return false;
    
    Symbol copy = symbol;
    if (symbol.binding() == Symbol::Auto)
        copy.setBinding(m_up ? Symbol::Local : Symbol::Global);

    m_symbols.push_back(copy);

    return true;
}

bool Symtab::find(std::string const& name, FindResult* res) const
{
    auto it = M_find(name, res);
    if (it != m_symbols.end())
        return true;

    if (res)
        ++res->locality;

    return m_up ? m_up->find(name, res) : false;
}

Symtab::const_iterator Symtab::begin() const
{ return m_symbols.begin(); }

Symtab::const_iterator Symtab::end() const
{ return m_symbols.end(); }

std::size_t Symtab::localsCount() const
{
    std::size_t count = 0;
    for (auto it = m_symbols.begin(); it != m_symbols.end(); ++it)
    {
        if (it->binding() == Symbol::Local &&
            it->which() != Symbol::Argument)
            ++count;
    }

    return count;
}

std::size_t Symtab::argumentsCount() const
{
    std::size_t count = 0;
    for (auto it = m_symbols.begin(); it != m_symbols.end(); ++it)
    {
        if (it->which() == Symbol::Argument)
            ++count;
    }

    return count;
}

Symtab::const_iterator Symtab::M_find(std::string const& name, FindResult* res) const
{
    std::size_t index = 0;
    std::size_t args_count = 0;
    for (const_iterator it = m_symbols.begin(); it != m_symbols.end(); ++it, ++index)
    {
        if (it->which() == Symbol::Argument)
            ++args_count;

        if (it->name() == name)
        {
            if (res)
            {
                res->symbol = const_cast<Symbol*>(&*it);
                res->index = index;
                res->args_count = args_count;
            }
            return it;
        }
    }

    return m_symbols.end();
}
