#include "lang/symbol.hpp"

using namespace lang;
using namespace core;

Symbol::Symbol(Symbol::Which which, Symbol::Binding binding, std::string const& name, Object data)
    : m_which(which)
    , m_binding(binding)
    , m_name(name)
    , m_data(data)
{}

Symbol::~Symbol()
{}

Symbol::Which Symbol::which() const
{ return m_which; }

Symbol::Binding Symbol::binding() const
{ return m_binding; }

void Symbol::setBinding(Symbol::Binding binding)
{ m_binding = binding; }

std::string const& Symbol::name() const
{ return m_name; }

Object Symbol::data() const
{ return m_data; }

Object& Symbol::data()
{ return m_data; }
