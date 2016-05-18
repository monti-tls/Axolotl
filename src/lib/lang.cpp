#include "lib/lang.hpp"
#include "core/core.hpp"
#include "vm/module.hpp"
#include "lang/lang.hpp"
#include "vm/import_table.hpp"

using namespace lib;
using namespace core;
using namespace lang;
using namespace vm;

void Lang::record()
{
    Module this_module = Module("lang");

    ImportTable::addBuiltin(this_module);

    this_module.global(std_main) = [](){};
    
    this_module.global("Token") = ObjectFactory::record<Token>("lang", "Token",
        ObjectFactory::constructorList()
        ([](int which, Object const& what) { return Token(which, what); }),
        ObjectFactory::methodList()
        (std_equals, [](Token const& tok, Token const& other) { return tok.which() == other.which(); })
        (std_equals, [](Token const& tok, int which)          { return tok.which() == which; })
        ("which",    [](Token const& tok)                     { return tok.which(); })
        ("what",     [](Token const& tok)                     { return tok.what(); })
        ("line",     [](Token const& tok)                     { return tok.where().line; })
        ("col",      [](Token const& tok)                     { return tok.where().col; })
        ("filename", [](Token const& tok)                     { return tok.where().filename; }));
}
