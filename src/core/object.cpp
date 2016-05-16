#include "core/object.hpp"
#include "core/object_impl.hpp"
#include "core/callable.hpp"
#include "core/callback_impl.hpp"
#include "lang/std_names.hpp"

using namespace core;

Object Object::m_nil = Object();
bool Object::m_nil_inited = false;

Object::Object()
    : m_weak(false)
    , m_impl(new Impl())
{
    m_impl->kind = Kind::Nil;
    m_impl->meta = Some();
    m_impl->classname = lang::std_nil_classname;
    m_impl->refcount = 1;
}

Object::Object(Object const& cpy)
{
    m_impl = cpy.m_impl;
    m_weak = cpy.m_weak;
    M_incref();
}

Object::Object(Object const& cpy, bool weaken)
{
    m_impl = cpy.m_impl;
    m_weak = weaken ? true : cpy.m_weak;
    M_incref();
}

Object::Object(Object::Kind kind, Some meta, std::string const& classname)
    : m_weak(false)
    , m_impl(new Impl())
{
    m_impl->kind = kind;
    m_impl->meta = meta;
    m_impl->classname = classname;
    m_impl->refcount = 1;
}

Object::~Object()
{
    M_decref();
}

Object& Object::operator=(Object const& cpy)
{
    M_decref();
    m_weak = cpy.m_weak;
    m_impl = cpy.m_impl;
    M_incref();

    return *this;
}

bool Object::isWeak() const
{ return m_weak; }

Object Object::weakref() const
{ return Object(*this, true); }

Object::Kind Object::kind() const
{ return m_impl->kind; }

Some const& Object::meta() const
{ return m_impl->meta; }

bool Object::isScalar() const
{ return m_impl->kind == Kind::Scalar; }

bool Object::isCallable() const
{ return m_impl->kind == Kind::Callable; }

bool Object::isInvokable() const
{ return isCallable() || has(lang::std_call); }

bool Object::isNil() const
{ return classname() == lang::std_nil_classname; }

std::string Object::classname() const
{ return m_impl->classname; }

bool Object::has(std::string const& id) const
{ return m_impl->members.count(id) >= 1; }

bool Object::isPolymorphic(std::string const& id) const
{ return m_impl->members.count(id) > 1; }

Object& Object::newPolymorphic(std::string const& id)
{ return m_impl->members.insert(std::pair<std::string, Object>(id, Object::nil()))->second; }

Object const& Object::findPolymorphic(std::string const& id, std::vector<Object> const& args) const
{
    auto range = m_impl->members.equal_range(id);

    for (auto it = range.first; it != range.second; ++it)
    {
        Object& obj = it->second;
        if (!obj.isCallable())
            throw std::runtime_error("core::Object::findPolymorphic: polymorphic member is not callable");
        if (obj.unwrap<Callable>().signature().match(args))
            return obj;
    }

    return nil();
}

Object& Object::member(std::string const& id)
{
    if (isPolymorphic(id))
        throw std::runtime_error("core::Object::member: member '" + id + "' is polymorphic");

    if (!has(id))
        m_impl->members.insert(std::pair<std::string, Object>(id, Object::nil()));

    return m_impl->members.find(id)->second;
}

Object const& Object::member(std::string const& id) const
{
    if (isPolymorphic(id))
        throw std::runtime_error("core::Object::member: member '" + id + "' is polymorphic");
    return m_impl->members.find(id)->second;
}

Object Object::invokeMember(std::string const& name, std::vector<Object> const& args) const
{
    if (!has(name))
    {
        throw std::runtime_error("member `" + name + "' does not exists in class `" + classname() + "'\n");
    }

    return member(name).invoke(args);
}

Object Object::invokePolymorphic(std::string const& name, std::vector<Object> const& args) const
{
    Object morph = findPolymorphic(name, args);
    if (morph.isNil())
        throw std::runtime_error("core::Object::findPolymorphic: no polymorphic member matches the current signature");
    return morph.invoke(args);
}

Object Object::invoke(std::vector<Object> const& args) const
{
    if (m_impl->kind == Kind::Callable)
    {
        if (!m_impl->meta.as<Callable>().signature().match(args))
            throw std::runtime_error("core::Object::invoke: signature mismatch");
	    return m_impl->meta.as<Callable>().invoke(args);
    }

    return invokePolymorphic(lang::std_call, args);
}

Object Object::method(std::string const& name, std::vector<Object> const& args) const
{
    std::vector<Object> new_args = { *this };
    std::copy(args.begin(), args.end(), std::back_inserter(new_args));

    Object morph = findPolymorphic(name, new_args);
    if (morph.isNil())
        throw std::runtime_error("core::Object:method: no polymorphic member matches the current signature");

    return morph.invoke(new_args);
}

Object Object::operator==(Object const& other) const
{
    if (other.isNil() && isNil())
        return true;

    if (other.isNil() || isNil())
        return false;

    return invokePolymorphic(lang::std_equals, { *this, other });
}

Object Object::operator!=(Object const& other) const
{
    if (other.isNil() && isNil())
        return false;

    if (other.isNil() || isNil())
        return true;

    return invokePolymorphic(lang::std_nequals, { *this, other });
}

Object Object::operator&&(Object const& other) const
{ return invokePolymorphic(lang::std_and, { *this, other}); }

Object Object::operator||(Object const& other) const
{ return invokePolymorphic(lang::std_or, { *this, other}); }

Object Object::operator!() const
{ return invokePolymorphic(lang::std_not, { *this }); }

Object Object::operator+(Object const& other) const
{ return invokePolymorphic(lang::std_add, { *this, other}); }

Object Object::operator-(Object const& other) const
{ return invokePolymorphic(lang::std_sub, { *this, other}); }

Object Object::operator*(Object const& other) const
{ return invokePolymorphic(lang::std_mul, { *this, other}); }

Object Object::operator/(Object const& other) const
{ return invokePolymorphic(lang::std_div, { *this, other}); }

Object Object::operator%(Object const& other) const
{ return invokePolymorphic(lang::std_mod, { *this, other}); }

Object Object::operator<(Object const& other) const
{ return invokePolymorphic(lang::std_lt, { *this, other }); }

Object Object::operator<=(Object const& other) const
{ return invokePolymorphic(lang::std_lte, { *this, other }); }

Object Object::operator>(Object const& other) const
{ return invokePolymorphic(lang::std_gt, { *this, other }); }

Object Object::operator>=(Object const& other) const
{ return invokePolymorphic(lang::std_gte, { *this, other }); }

Object::operator bool() const
{ return unwrap<bool>(); }

std::string Object::serialize() const
{ return invokeMember(lang::std_serialize, { *this }).unwrap<std::string>(); }

Object const& Object::nil()
{
    if (m_nil_inited)
    {
        m_nil_inited = true;
        setupBuiltinMembers(m_nil);
    }
    return m_nil;
}

void Object::setupBuiltinMembers(Object& obj)
{
    obj.newPolymorphic(lang::std_classname) = [](Object const& obj)
                                              { return obj.classname(); };

    obj.newPolymorphic(lang::std_lte)       = [](Object const& self, Object const& obj)
                                              { return (self < obj) || (self == obj); };

    obj.newPolymorphic(lang::std_gt)        = [](Object const& self, Object const& obj)
                                              { return obj < self; };

    obj.newPolymorphic(lang::std_gte)       = [](Object const& self, Object const& obj)
                                              { return (self > obj) || (self == obj); };

    obj.newPolymorphic(lang::std_nequals)   = [](Object const& self, Object const& obj)
                                              { return self != obj; };
}

void Object::M_incref()
{
    if (m_impl && !m_weak)
        ++m_impl->refcount;
}

void Object::M_decref()
{
    if (m_impl && !m_weak && !--m_impl->refcount)
    {
        M_destroy();
        delete m_impl;
    }
}

void Object::M_destroy()
{
    if (has(lang::std_del))
        invokeMember(lang::std_del, { weakref() });
}
