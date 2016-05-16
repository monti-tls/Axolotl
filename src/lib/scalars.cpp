#include "lib/scalars.hpp"
#include "core/core.hpp"
#include "lang/std_names.hpp"

#include <sstream>

using namespace lib;
using namespace core;
using namespace lang;

void Scalars::record()
{
    recordBool();
    recordInt();
    recordChar();
    recordString();
    recordFloat();
}

void Scalars::recordBool()
{
    ObjectFactory::record<Bool>("bool",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_and,       [](Bool a, Bool b) { return a && b; })
        (std_or,        [](Bool a, Bool b) { return a || b; })
        (std_not,       [](Bool a)         { return !a; })
        (std_equals,    [](Bool a, Bool b) { return a == b; })
        (std_lt,        [](Bool a, Bool b) { return a < b; })
        (std_serialize, [](Bool a)
        {
            std::ostringstream ss;
            ss << std::boolalpha;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            bool a;
            ss >> std::boolalpha >> a;
            return a;
        }));
}

void Scalars::recordInt()
{
    ObjectFactory::record<Int>("int",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_add,       [](Int a, Int b) { return a + b; })
        (std_sub,       [](Int a, Int b) { return a - b; })
        (std_mul,       [](Int a, Int b) { return a * b; })
        (std_div,       [](Int a, Int b) { return a / b; })
        (std_mod,       [](Int a, Int b) { return a % b; })
        (std_equals,    [](Int a, Int b) { return a == b; })
        (std_lt,        [](Int a, Int b) { return a < b; })
        (std_serialize, [](Int a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            int a;
            ss >> a;
            return a;
        }));
}

void Scalars::recordChar()
{
    ObjectFactory::record<Char>("char",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_equals,    [](Char a, Char b) { return a == b; })
        (std_lt,        [](Char a, Char b) { return a < b; })
        (std_serialize, [](Char a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            char a;
            ss >> a;
            return a;
        }));
}

void Scalars::recordString()
{
    ObjectFactory::record<String>("string",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_equals,      [](String const& a, String const& b) { return a == b; })
        (std_lt,          [](String const& a, String const& b) { return a < b; })
        (std_serialize,   [](Object const& o) { return o; })
        (std_unserialize, [](Object const& o) { return o; })
        ("append",        [](String& s, char c)      { s += c; })
        ("size",          [](String const& s)        { return (int) s.size(); })
        ("at",            [](String const& s, int i) { return s[i]; }));
}

void Scalars::recordFloat()
{
    ObjectFactory::record<Float>("float",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_add,       [](Float a, Float b) { return a + b; })
        (std_sub,       [](Float a, Float b) { return a - b; })
        (std_mul,       [](Float a, Float b) { return a * b; })
        (std_div,       [](Float a, Float b) { return a / b; })
        (std_mod,       [](Float a, Float b) { return std::fmod(a, b); })
        (std_equals,    [](Float a, Float b) { return a == b; })
        (std_lt,        [](Float a, Float b) { return a < b; })
        (std_serialize, [](Float a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            Float a;
            ss >> a;
            return a;
        }));
}
