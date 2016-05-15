#include "lang/xltl_symbol.hpp"

using namespace lang;
using namespace core;

XltlSymbol::XltlSymbol(XltlSymbol::Which which, std::string const& name, Object const& data)
    : m_which(which)
    , m_binding(XltlSymbol::Global)
    , m_name(name)
    , m_data(data)
{}

XltlSymbol::~XltlSymbol()
{}

XltlSymbol::Which XltlSymbol::which() const
{ return m_which; }

XltlSymbol::Binding XltlSymbol::binding() const
{ return m_binding; }

void XltlSymbol::setBinding(XltlSymbol::Binding binding)
{ m_binding = binding; }

std::string const& XltlSymbol::name() const
{ return m_name; }

Object const& XltlSymbol::data() const
{ return m_data; }

Object& XltlSymbol::data()
{ return m_data; }
