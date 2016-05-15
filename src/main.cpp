#include <iostream>
#include <iomanip>
#include <type_traits>
#include <cstring>
#include <string>
#include <map>
#include <iomanip>
#include <stack>
#include <sstream>
#include <fstream>

#include "core/core.hpp"
#include "bits/bits.hpp"
#include "vm/vm.hpp"
#include "lang/lang.hpp"
#include "util/util.hpp"
#include "lib/lib.hpp"

using namespace core;
using namespace bits;
using namespace vm;
using namespace lang;
using namespace nfa;
using namespace ast;
using namespace lib;

void registerCoreTypes()
{
    ObjectFactory::registerType(Signature::AnyTypeName,
        std::function<Object(Object const&)>([](Object const& o) { return o; }));
}

void registerBuiltinTypes()
{
    ObjectFactory::registerType<bool>("bool",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_and,       [](bool a, bool b) { return a && b; })
        (std_or,        [](bool a, bool b) { return a || b; })
        (std_not,       [](bool a) { return !a; })
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

    ObjectFactory::registerType<int>("int",
        ObjectFactory::constructorList(),
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

    ObjectFactory::registerType<float>("float",
        ObjectFactory::constructorList(),
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
            int a;
            ss >> a;
            return a;
        }));

    ObjectFactory::registerType<char>("char",
        ObjectFactory::constructorList(),
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

    ObjectFactory::registerType<std::string>("string",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_equals,      [](std::string const& a, std::string const& b) { return a == b; })
        (std_lt,          [](std::string const& a, std::string const& b) { return a < b; })
        (std_serialize,   [](Object const& o) { return o; })
        (std_unserialize, [](Object const& o) { return o; })
        ("append",        [](std::string& s, char c) { s += c; })
        ("size",          [](std::string const& s) { return (int) s.size(); })
        ("at",            [](std::string const& s, int i) { return s[i]; }));

    lib::Dict::registerType();
}

void registerOtherTypes()
{
    ObjectFactory::registerType<vm::StackFrame>("StackFrame",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        ("pc",           [](StackFrame const& sf) { return sf.pc; })
        ("locals_count", [](StackFrame const& sf) { return sf.locals_count; })
        ("locals_start", [](StackFrame const& sf) { return sf.locals_start; })
        ("argc",         [](StackFrame const& sf) { return sf.argc; }));

    ObjectFactory::registerType<Token>("Token",
        ObjectFactory::constructorList()
        ([](int which, Object const& what) { return Token(which, what); }),
        ObjectFactory::methodList()
        (std_equals, [](Token const& tok, Token const& other) { return tok.which() == other.which(); })
        (std_equals, [](Token const& tok, int which) { return tok.which() == which; })
        ("which",    [](Token const& tok) { return tok.which(); })
        ("what",     [](Token const& tok) { return tok.what(); })
        ("line",     [](Token const& tok) { return tok.where().line; })
        ("col",      [](Token const& tok) { return tok.where().col; })
        ("filename", [](Token const& tok) { return tok.where().filename; }));
}

void registerAllTypes()
{
    registerCoreTypes();
    registerBuiltinTypes();
    registerOtherTypes();
}

int main()
{
    registerAllTypes();

    std::string str =
    /*"globl = 123;\n"
    "dummy = 0;\n"
    "fun add(a : int, b : int)\n"
    "{\n"
    "    locl = 0;\n"
    "    locl.memb = 5;\n"
    "    globl = globl.memb + 1;\n"
    "    fun mul(c, d)\n"
    "        return c * d;\n"
    "    dummy();\n"
    "    return mul + b + locl;\n"
    "}\n"
    "fun sub(a, b)\n"
    "    return a - b;\n\n";*/
    "dummy = 0;\n"
    "dummy().c;\n"
    "dummy.c = dummy();\n"
    ;

    std::istringstream ss;
    ss.str(str);

    Compiler* compiler = new Compiler(ss);

    compiler->compile();

    delete compiler;

    return 0;
}
