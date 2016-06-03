#include "lib/core.hpp"
#include "core/core.hpp"
#include "vm/module.hpp"
#include "lang/std_names.hpp"
#include "vm/stack_frame.hpp"
#include "vm/import_table.hpp"

#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace lib;
using namespace core;
using namespace lang;
using namespace vm;

void Core::record()
{
    Module this_module = Module("core");
    ImportTable::addBuiltin(this_module);

    /*ObjectFactory::m_impl->classes[detail::uniqueTypeId<Callable>()] = Class::CallableClass;*/

    this_module.global(std_main) = [](){};

    {
        Class c("core", lang::std_nil_classname);
        this_module.global(c.classname()) = c;
        associate<void>(c);
    }
    {
        Class c("core", lang::std_callable_classname);
        this_module.global(c.classname()) = c;
        associate<Callable>(c);
    }
    {
        Class c("core", "Class");
        c["classname"] = &Class::classname;
        c["unserialize"] = &Class::unserialize;
        this_module.global(c.classname()) = c;
        associate<Class>(c);
    }
    {
        Class c("core", "bool");
        c["bool"]     = [](bool a) { return a; };
        c[std_and]    = [](bool a, bool b) { return a && b; };
        c[std_or]     = [](bool a, bool b) { return a || b; };
        c[std_not]    = [](bool a) { return !a; };
        c[std_equals] = [](bool a, bool b) { return a == b; };
        c[std_lt]     = [](bool a, bool b) { return a < b; };
        c[std_serialize] = [](bool a)
        {
            std::ostringstream ss;
            ss << std::boolalpha << a;
            return ss.str();
        };
        c[std_unserialize] = [](std::string const& str)
        {
            bool a;
            std::istringstream(str) >> std::boolalpha >> a;
            return a;
        };
        this_module.global(c.classname()) = c;
        associate<bool>(c);
    }
    {
        Class c("core", "int");
        c["int"]      = [](int a) { return a; };
        c[std_add]    = [](int a, int b) { return a + b; };
        c[std_sub]    = [](int a, int b) { return a - b; };
        c[std_mul]    = [](int a, int b) { return a * b; };
        c[std_div]    = [](int a, int b) { return a / b; };
        c[std_mod]    = [](int a, int b) { return a % b; };
        c[std_neg]    = [](int a)        { return -a; };
        c[std_equals] = [](int a, int b) { return a == b; };
        c[std_lt]     = [](int a, int b) { return a < b; };
        c[std_serialize] = [](int a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        };
        c[std_unserialize] = [](std::string const& str)
        {
            int a;
            std::istringstream(str) >> a;
            return a;
        };
        this_module.global(c.classname()) = c;
        associate<int>(c);
    }
    {
        Class c("core", "ulong");
        c["ulong"]    = [](std::size_t a) { return a; };
        c[std_add]    = [](std::size_t a, std::size_t b) { return a + b; };
        c[std_sub]    = [](std::size_t a, std::size_t b) { return a - b; };
        c[std_mul]    = [](std::size_t a, std::size_t b) { return a * b; };
        c[std_div]    = [](std::size_t a, std::size_t b) { return a / b; };
        c[std_mod]    = [](std::size_t a, std::size_t b) { return a % b; };
        c[std_neg]    = [](std::size_t a)                { return -a; };
        c[std_equals] = [](std::size_t a, std::size_t b) { return a == b; };
        c[std_lt]     = [](std::size_t a, std::size_t b) { return a < b; };
        c[std_serialize] = [](std::size_t a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        };
        c[std_unserialize] = [](std::string const& str)
        {
            std::size_t a;
            std::istringstream(str) >> a;
            return a;
        };
        this_module.global(c.classname()) = c;
        associate<std::size_t>(c);
    }
    {
        Class c("core", "float");
        c["float"]      = [](float a) { return a; };
        c[std_add]    = [](float a, float b) { return a + b; };
        c[std_sub]    = [](float a, float b) { return a - b; };
        c[std_mul]    = [](float a, float b) { return a * b; };
        c[std_div]    = [](float a, float b) { return a / b; };
        c[std_mod]    = [](float a, float b) { return std::fmod(a , b); };
        c[std_neg]    = [](float a)          { return -a; };
        c[std_equals] = [](float a, float b) { return a == b; };
        c[std_lt]     = [](float a, float b) { return a < b; };
        c[std_serialize] = [](float a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        };
        c[std_unserialize] = [](std::string const& str)
        {
            float a;
            std::istringstream(str) >> a;
            return a;
        };
        this_module.global(c.classname()) = c;
        associate<float>(c);
    }
    {
        Class c("core", "char");
        c["char"]     = [](char a) { return a; };
        c[std_sub]    = [](char a, char b) { return (int) (a - b); };
        c[std_equals] = [](char a, char b) { return a == b; };
        c[std_lt]     = [](char a, char b) { return a < b; };
        c[std_serialize] = [](char a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        };
        c[std_unserialize] = [](std::string const& str)
        {
            char a;
            std::istringstream(str) >> a;
            return a;
        };
        this_module.global(c.classname()) = c;
        associate<char>(c);
    }
    {
        Class c("core", "string");
        c["string"]      = [](std::string const& a) { return a; };
        c[std_add]       = [](std::string self, std::string const& b) { self += b; return self; };
        c[std_equals]    = [](std::string const& a, std::string const& b) { return a == b; };
        c[std_lt]        = [](std::string const& a, std::string const& b) { return a < b; };
        c["get"]         = [](std::string const& a, int i) { return a.at(i); };
        c["size"]        = [](std::string const& a) { return (int) a.size(); };
        c["set"]         = [](std::string& a, int i, char c) { a[i] = c; };
        c["append"]      = [](std::string& a, char c) { a += c; };
        c["extend"]      = [](std::string& a, std::string const& b) { a += b; };
        c[std_serialize] = [](std::string const& a)
        {
            return a;
        };
        c[std_unserialize] = [](std::string const& str)
        {
            return str;
        };
        this_module.global(c.classname()) = c;
        associate<std::string>(c);
    }
    {
        Class c("core", "StackFrame");
        this_module.global(c.classname()) = c;
        associate<StackFrame>(c);
    }
    {
        Class c("core", "list");
        c["list"] = []()
        {
            return std::vector<Object>();
        };
        c["list"] = [](Object const& obj)
        {
            return std::vector<Object>(obj);
        };
        c["list"] = [](int len)
        {
            return std::vector<Object>(len, Object::nil());
        };
        c["list"] = [](int len, Object const& obj)
        {
            return std::vector<Object>(len, obj);
        };
        c["get"] = [](std::vector<Object> const& self, int i)
        {
            return self.at(i);
        };
        c["set"] = [](std::vector<Object>& self, int i, Object const& obj)
        {
            self.at(i) = obj;
        };
        c["size"] = [](std::vector<Object>& self)
        {
            return (int) self.size();
        };
        c["append"] = [](std::vector<Object>& self, Object const& obj)
        {
            self.push_back(obj);
        };
        c["extend"] = [](std::vector<Object>& self, std::vector<Object> const& list)
        {
            std::copy(list.begin(), list.end(), std::back_inserter(self));
        };
        c["swap"] = [](std::vector<Object>& self, int i, int j)
        {
            Object temp = self.at(i);
            self[i] = self.at(j);
            self[j] = temp;
        };
        c["apply"] = [](std::vector<Object>& self, Object const& function)
        {
            for (auto& elem : self)
                elem = function(elem);
        };
        this_module.global(c.classname()) = c;
        associate<std::vector<Object>>(c);
    }
}
