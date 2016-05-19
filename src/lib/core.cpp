#include "lib/core.hpp"
#include "core/core.hpp"
#include "vm/module.hpp"
#include "lang/std_names.hpp"
#include "vm/stack_frame.hpp"
#include "vm/import_table.hpp"

#include <sstream>

using namespace lib;
using namespace core;
using namespace lang;
using namespace vm;

void Core::record()
{
    Module this_module = Module("core");
    ImportTable::addBuiltin(this_module);

    /*ObjectFactory::m_impl->classes[detail::uniqueTypeId<Callable>()] = Class::CallableClass;*/

    this_module.global("Nil") = ObjectFactory::record<void>("core", "Nil",
        ObjectFactory::constructorList()
        ([](){ return Object::nil(); }),
        ObjectFactory::methodList());

    this_module.global("Callable") = ObjectFactory::record<Callable>("core", "Callable",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList());

    this_module.global(std_main) = [](){};

    this_module.global("Class") = ObjectFactory::record<Class>("core", "Class",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        ("classname",   &Class::classname)
        ("unserialize", &Class::unserialize));

    this_module.global("bool") = ObjectFactory::record<bool>("core", "bool",
        ObjectFactory::constructorList()
        ([](bool a) { return a; }),
        ObjectFactory::methodList()
        (std_and,       [](bool a, bool b) { return a && b; })
        (std_or,        [](bool a, bool b) { return a || b; })
        (std_not,       [](bool a)         { return !a; })
        (std_equals,    [](bool a, bool b) { return a == b; })
        (std_lt,        [](bool a, bool b) { return a < b; })
        (std_serialize, [](bool a)
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

    this_module.global("int") = ObjectFactory::record<int>("core", "int",
        ObjectFactory::constructorList()
        ([](int a) { return a; }),
        ObjectFactory::methodList()
        (std_add,       [](int a, int b) { return a + b; })
        (std_sub,       [](int a, int b) { return a - b; })
        (std_mul,       [](int a, int b) { return a * b; })
        (std_div,       [](int a, int b) { return a / b; })
        (std_mod,       [](int a, int b) { return a % b; })
        (std_equals,    [](int a, int b) { return a == b; })
        (std_lt,        [](int a, int b) { return a < b; })
        (std_serialize, [](int a)
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

    this_module.global("char") = ObjectFactory::record<char>("core", "char",
        ObjectFactory::constructorList()
        ([](char a) { return a; }),
        ObjectFactory::methodList()
        (std_equals,    [](char a, char b) { return a == b; })
        (std_lt,        [](char a, char b) { return a < b; })
        (std_serialize, [](char a)
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

    this_module.global("string") = ObjectFactory::record<std::string>("core", "string",
        ObjectFactory::constructorList()
        ([](std::string const& a) { return a; }),
        ObjectFactory::methodList()
        (std_equals,      [](std::string const& a, std::string const& b) { return a == b; })
        (std_lt,          [](std::string const& a, std::string const& b) { return a < b; })
        (std_serialize,   [](Object const& o) { return o; })
        (std_unserialize, [](Object const& o) { return o; })
        ("append",        [](std::string& s, char c)      { s += c; })
        ("size",          [](std::string const& s)        { return (int) s.size(); })
        ("at",            [](std::string const& s, int i) { return s[i]; }));

    this_module.global("float") = ObjectFactory::record<float>("core", "float",
        ObjectFactory::constructorList()
        ([](float a) { return a; }),
        ObjectFactory::methodList()
        (std_add,       [](float a, float b) { return a + b; })
        (std_sub,       [](float a, float b) { return a - b; })
        (std_mul,       [](float a, float b) { return a * b; })
        (std_div,       [](float a, float b) { return a / b; })
        (std_mod,       [](float a, float b) { return std::fmod(a, b); })
        (std_equals,    [](float a, float b) { return a == b; })
        (std_lt,        [](float a, float b) { return a < b; })
        (std_serialize, [](float a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            float a;
            ss >> a;
            return a;
        }));

    this_module.global("ulong") = ObjectFactory::record<std::size_t>("core", "ulong",
        ObjectFactory::constructorList()
        ([](std::size_t a) { return a; }),
        ObjectFactory::methodList()
        (std_add,       [](std::size_t a, std::size_t b) { return a + b; })
        (std_sub,       [](std::size_t a, std::size_t b) { return a - b; })
        (std_mul,       [](std::size_t a, std::size_t b) { return a * b; })
        (std_div,       [](std::size_t a, std::size_t b) { return a / b; })
        (std_mod,       [](std::size_t a, std::size_t b) { return a % b; })
        (std_equals,    [](std::size_t a, std::size_t b) { return a == b; })
        (std_lt,        [](std::size_t a, std::size_t b) { return a < b; })
        (std_serialize, [](std::size_t a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            std::size_t a;
            ss >> a;
            return a;
        }));

    this_module.global("StackFrame") = ObjectFactory::record<StackFrame>("core", "StackFrame",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        ("pc",           [](StackFrame const& sf) { return sf.pc; })
        ("locals_count", [](StackFrame const& sf) { return sf.locals_count; })
        ("locals_start", [](StackFrame const& sf) { return sf.locals_start; })
        ("argc",         [](StackFrame const& sf) { return sf.argc; }));

    this_module.global("list") = ObjectFactory::record<std::vector<Object>>("core", "list",
        ObjectFactory::constructorList()
        ([]() { return std::vector<Object>(); })
        ([](Object const& obj) { return std::vector<Object>(obj); })
        ([](int len) { return std::vector<Object>(len, Object::nil()); })
        ([](int len, Object const& obj) { return std::vector<Object>(len, obj); }),
        ObjectFactory::methodList()
        ("get", [](std::vector<Object>& self, int i)
        {
            if (i < 0 || i >= (int) self.size())
                return Object::nil();
            return self[i];
        })
        ("set", [](std::vector<Object>& self, int i, Object const& obj)
        {
            if (i < 0 || i >= (int) self.size())
                return;
            self[i] = obj;
        })
        ("size", [](std::vector<Object> const& self) { return (int) self.size(); })
        ("append", [](std::vector<Object>& self, Object const& obj)
        { self.push_back(obj); })
        ("extend", [](std::vector<Object>& self, std::vector<Object> const& other)
        { self.insert(self.end(), other.begin(), other.end()); }));

}
