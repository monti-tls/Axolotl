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

int main()
{
    std::ifstream ss("./sample.xl");
    Compiler* compiler = new Compiler("main", ss);
    Module module(compiler->compile());
    delete compiler;

    Engine engine(module);

    std::cout << "__main__() = " << module.global("__main__")().unwrap<int>() << std::endl;

    return 0;
}
