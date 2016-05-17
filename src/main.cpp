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

class YoloModulo
{
public:
    YoloModulo(int a)
        : m_a(a)
    {
        std::cout << "ctor(" << a << ")" << std::endl;
    }

    YoloModulo(YoloModulo const& cpy)
    {
        std::cout << "copy ctor" << std::endl;
        m_a = cpy.m_a;
    }

    ~YoloModulo()
    {}

    int getA()
    { return m_a; }

    YoloModulo doubled()
    { return YoloModulo(2 * m_a); }

private:
    int m_a;
};

int main()
{
    try
    {
        Module module("my");
        module.global("YoloModulo") = ObjectFactory::record<YoloModulo>(
            "YoloModulo",
            ObjectFactory::constructorList()
            ([](int a) { return YoloModulo(a); }),
            ObjectFactory::methodList()
            ("getA", &YoloModulo::getA)
            ("doubled", &YoloModulo::doubled));

        Script script;
        script.addModule(module);
        script.fromFile("./sample.xl");

        Object obj = script.run("__main__");
        std::cout << obj.unwrap<int>() << std::endl;
    }
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << std::endl;
        return -1;
    }

    return 0;
}
