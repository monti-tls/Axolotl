#include "lib/dict.hpp"
#include "lang/std_names.hpp"

using namespace lib;
using namespace core;
using namespace lang;

void Dict::record()
{
    
}

Dict::Dict()
{}

Dict::~Dict()
{}

Dict::iterator Dict::begin()
{ return m_impl.begin(); }

Dict::const_iterator Dict::begin() const
{ return m_impl.begin(); }

Dict::iterator Dict::end()
{ return m_impl.end(); }

Dict::const_iterator Dict::end() const
{ return m_impl.end(); }

Dict::iterator Dict::find(Object key)
{ return m_impl.find(key); }

Dict::const_iterator Dict::find(Object key) const
{ return m_impl.find(key); }

void Dict::set(Object key, Object value)
{ m_impl[key] = value; }
