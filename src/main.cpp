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

void print(Object const& o)
{
    if (o.meta().is<std::size_t>())
        std::cout << "[ulong: " << o.unwrap<std::size_t>() << "]" << std::endl;
    else if (o.meta().is<std::string>())
        std::cout << o.unwrap<std::string>() << std::endl;
    else if (o.meta().is<int>())
        std::cout << o.unwrap<int>() << std::endl;
}

int main()
{
    /*try
    {*/
        Module module("my");
        module.global("print") = &print;

        Script script;
        script.addModule(module);
        script.fromFile("./sample.xl");

        script.run("__main__");
    /*}
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << std::endl;
        return -1;
    }*/

    return 0;
}
