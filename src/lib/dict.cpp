#include "lib/dict.hpp"
#include "lang/std_names.hpp"

using namespace lib;
using namespace core;
using namespace lang;

void Dict::record()
{
    ObjectFactory::record<Dict>("dict",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        ("find",          [](Dict const& dict, Object const& key)
        {
            auto it = dict.find(key);
            if (it != dict.end())
                return (Object) it->second;
            return Object::nil();
        })
        ("insert",        [](Dict& dict, Object const& key, Object const& value)
        { dict.set(key, value); }));
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

Dict::iterator Dict::find(Object const& key)
{ return m_impl.find(key); }

Dict::const_iterator Dict::find(Object const& key) const
{ return m_impl.find(key); }

void Dict::set(Object const& key, Object const& value)
{ m_impl[key] = value; }
