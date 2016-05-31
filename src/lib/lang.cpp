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

    Class c("lang", "Token");
    this_module.global(c.classname()) = c;
    associate<Token>(c);
}
