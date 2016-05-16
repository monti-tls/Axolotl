#include "lib/core.hpp"
#include "core/core.hpp"
#include "lang/std_names.hpp"
#include "vm/stack_frame.hpp"

using namespace lib;
using namespace core;
using namespace lang;
using namespace vm;

void Core::record()
{
    ObjectFactory::record(std_any_type,
        std::function<Object(Object const&)>([](Object const& o) { return o; }));

    ObjectFactory::record<StackFrame>("StackFrame",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        ("pc",           [](StackFrame const& sf) { return sf.pc; })
        ("locals_count", [](StackFrame const& sf) { return sf.locals_count; })
        ("locals_start", [](StackFrame const& sf) { return sf.locals_start; })
        ("argc",         [](StackFrame const& sf) { return sf.argc; }));
}
