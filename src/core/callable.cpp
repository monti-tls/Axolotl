#include "core/callable.hpp"
#include "vm/vm.hpp"

using namespace core;

Callable::Callable(Callable::ScriptedMetaType const& scripted)
    : m_kind(Kind::Scripted)
    , m_meta(scripted)
{}

Callable::~Callable()
{}

Callable::Kind Callable::kind() const
{ return m_kind; }

Some const& Callable::meta() const
{ return m_meta; }

Object Callable::invoke(std::vector<Object> const& args) const
{
    if (m_kind == Kind::Native)
        return m_meta.as<NativeMetaType>()->invoke(args);
    else // if (m_kind == Scripted)
        return m_meta.as<ScriptedMetaType>().invoke(args);
}

Signature Callable::signature() const
{
    if (m_kind == Kind::Native)
        return m_meta.as<NativeMetaType>()->signature();
    else // if (m_kind == Scripted)
        return m_meta.as<ScriptedMetaType>().signature();
}
