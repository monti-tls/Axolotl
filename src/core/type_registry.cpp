#include "core/type_registry.hpp"
#include "lib/lib.hpp"
#include "lang/std_names.hpp"

namespace core
{
    namespace detail
    {
        std::map<std::size_t, Class>* type_registry = nullptr;

        static void __attribute__((constructor)) type_registry_init()
        {
            type_registry = new std::map<std::size_t, Class>();

            Class::AnyClass = Class(lang::std_any_classname, lang::std_core_module_name);
            Class::AnyId = Class::AnyClass.classid();
            type_registry->insert(std::make_pair(uniqueTypeId<Object>(), Class::AnyClass));

            lib::Core::record();
            lib::Lang::record();
        }

        static void __attribute__((destructor)) type_registry_fini()
        {
            delete type_registry;
        }
    }
}
