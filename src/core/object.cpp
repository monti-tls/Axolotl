#include "core/object.hpp"
#include "core/object_impl.hpp"
#include "core/callable.hpp"
#include "core/callback_impl.hpp"
#include "core/exception.hpp"
#include "lang/std_names.hpp"

using namespace core;

Object::Object()
    : m_weak(false)
    , m_impl(new Impl())
{
    m_impl->meta = Some();
    m_impl->pending_type_id = detail::uniqueTypeId<void>();
    m_impl->pending = true;
    m_impl->refcount = 1;
}

Object::Object(Object const& cpy, bool weaken)
{
    m_impl = cpy.m_impl;
    m_weak = weaken ? true : cpy.m_weak;
    M_incref();
}

Object::Object(Some&& meta, Class const& the_class)
    : m_weak(false)
    , m_impl(new Impl())
{
    m_impl->meta = std::move(meta);
    m_impl->the_class = the_class;
    m_impl->pending = false;
    m_impl->refcount = 1;
}

Object::Object(Some&& meta, std::size_t pending_type_id)
    : m_weak(false)
    , m_impl(new Impl())
{
    m_impl->meta = std::move(meta);
    m_impl->pending = true;
    m_impl->pending_type_id = pending_type_id;
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

bool Object::weak() const
{
    return m_weak;
}

Object Object::weakref() const
{
    M_fixPending();
    return Object(*this, true);
}

Object Object::copy() const
{
    Object cpy;
    *cpy.m_impl = *m_impl;
    cpy.m_impl->refcount = 1;
    return cpy;
}

Some const& Object::meta() const
{
    M_fixPending();
    return m_impl->meta;
}

bool Object::pending() const
{ return m_impl->pending; }

bool Object::callable() const
{
    M_fixPending();
    return m_impl->meta.is<Callable>();
}

bool Object::invokable() const
{
    M_fixPending();
    return callable() || has(lang::std_call);
}

bool Object::isNil() const
{
    M_fixPending();
    return classname() == lang::std_nil_classname;
}

Class const& Object::theClass() const
{
    M_fixPending();
    return m_impl->the_class;
}

std::string Object::classname() const
{ return theClass().classname(); }

Class::Id Object::classid() const
{ return theClass().classid(); }

bool Object::has(std::string const& id) const
{
    M_fixPending();
    return m_impl->members.count(id) >= 1;
}

bool Object::isPolymorphic(std::string const& id) const
{
    M_fixPending();
    return m_impl->members.count(id) > 1;
}

Object& Object::newPolymorphic(std::string const& id)
{
    M_fixPending();
    return m_impl->members.insert(std::pair<std::string, Object>(id, Object::nil()))->second;
}

Object Object::findPolymorphic(std::string const& id, std::vector<Object> const& args) const
{
    M_fixPending();
    auto range = m_impl->members.equal_range(id);

    // Search backward to follow the 'least specialized first' rule
    for (auto it = --range.second; ; --it)
    {
        Object& obj = it->second;
        if (!obj.callable())
        {
            throw ClassError(obj, type_class<Callable>());
            // throw std::runtime_error("core::Object::findPolymorphic: polymorphic member is not callable");
        }
        if (obj.unwrap<Callable>().signature().match(args))
            return obj;

        if (it == range.first)
            break;
    }

    return nil();
}

Object& Object::member(std::string const& id)
{
    M_fixPending();
    if (isPolymorphic(id))
    {
        throw NoMemberError(*this, id);
        // throw std::runtime_error("core::Object::member: member '" + id + "' is polymorphic");
    }

    if (!has(id))
        m_impl->members.insert(std::pair<std::string, Object>(id, Object::nil()));

    return m_impl->members.find(id)->second;
}

Object const& Object::member(std::string const& id) const
{
    M_fixPending();
    if (isPolymorphic(id))
    {
        throw NoMemberError(*this, id);
        // throw std::runtime_error("core::Object::member: member '" + id + "' is polymorphic");
    }

    auto it = m_impl->members.find(id);
    if (it == m_impl->members.end())
        throw NoMemberError(*this, id);

    return it->second;
}

Object Object::invokeMember(std::string const& name, std::vector<Object> const& args) const
{
    M_fixPending();
    if (!has(name))
    {
        throw NoMemberError(*this, name);
        // throw std::runtime_error("member `" + name + "' does not exists in class `" + classname() + "'\n");
    }

    return member(name).invoke(args);
}

Object Object::invokePolymorphic(std::string const& name, std::vector<Object> const& args) const
{
    M_fixPending();
    Object morph = findPolymorphic(name, args);
    if (morph.isNil())
    {
        throw SignatureError(*this, name, args);
        // throw std::runtime_error("core::Object::findPolymorphic: no polymorphic member matches the current signature");
    }
    return morph.invoke(args);
}

Object Object::invoke(std::vector<Object> const& args) const
{
    M_fixPending();
    if (callable())
    {
        if (!m_impl->meta.as<Callable>().signature().match(args))
        {
            throw SignatureError(*this, "", args);
            // throw std::runtime_error("core::Object::invoke: signature mismatch");
        }
	    return m_impl->meta.as<Callable>().invoke(args);
    }

    std::vector<Object> cpy = args;
    cpy.insert(cpy.begin(), *this);
    return invokePolymorphic(lang::std_call, cpy);
}

Object Object::method(std::string const& name, std::vector<Object> const& args) const
{
    M_fixPending();
    std::vector<Object> new_args = { *this };
    std::copy(args.begin(), args.end(), std::back_inserter(new_args));

    Object morph = findPolymorphic(name, new_args);
    if (morph.isNil())
    {
        throw SignatureError(*this, name, args);
        // throw std::runtime_error("core::Object:method: no polymorphic member matches the current signature");
    }

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

Object Object::nil()
{ return Object(); }

void Object::M_incref()
{
    if (m_impl && !m_weak)
        ++m_impl->refcount;
}

void Object::M_decref()
{
    if (m_impl && !m_weak && !--m_impl->refcount)
    {
        if (!m_impl->pending)
            M_destroy();
        delete m_impl;
    }
}

void Object::M_destroy()
{
    if (has(lang::std_del))
        invokeMember(lang::std_del, { weakref() });
}

void Object::M_fixPending() const
{
    if (!m_impl || !m_impl->pending)
        return;

    Class const& the_class = type_class(m_impl->pending_type_id);
    *const_cast<Object*>(this) = the_class.construct(Some(m_impl->meta));
}
