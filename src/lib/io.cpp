#include "lib/io.hpp"
#include "core/core.hpp"
#include "vm/module.hpp"
#include "lang/std_names.hpp"
#include "vm/stack_frame.hpp"
#include "vm/import_table.hpp"

#include <sstream>
#include <vector>
#include <algorithm>

using namespace lib;
using namespace core;
using namespace lang;
using namespace vm;

void Io::put(Object obj)
{
    if (obj.meta().is<std::string>())
    {
        std::cout << obj.unwrap<std::string>();
    }
    else if (obj.meta().is<std::vector<Object>>())
    {
        std::vector<Object> const& list = obj.unwrap<std::vector<Object>>();
        std::cout << "[";
        for (int i = 0; i < (int) list.size(); ++i)
        {
            put(list[i]);
            if (i != (int) list.size()-1)
                std::cout << ", ";
        }
        std::cout << "]";
    }
    else
        std::cout << obj.serialize();
}

void Io::record()
{
    Module this_module = Module("io");
    ImportTable::addBuiltin(this_module);

    this_module.global(std_main) = [](){};

    this_module.global("put") = [](Object obj)
    { Io::put(obj); };

    this_module.global("putln") = [](Object obj)
    { Io::put(obj); std::cout << std::endl; };

    auto print = [](std::vector<Object> list)
    { for (auto elem : list) Io::put(elem); };
    
    this_module.global("print") = Callable(std::function<void(std::vector<Object>)>(print), true);

    auto println = [&print](std::vector<Object> list)
    { print(list); std::cout << std::endl; };
    
    this_module.global("println") = Callable(std::function<void(std::vector<Object>)>(println), true);
}
