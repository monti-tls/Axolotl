#include "core/object_factory.hpp"
#include "lib/lib.hpp"

namespace core
{
    ObjectFactory::Impl* ObjectFactory::m_impl = nullptr;

    namespace detail
    {
        static void __attribute__((constructor)) ObjectFactory_init()
        {
            ObjectFactory::m_impl = new ObjectFactory::Impl();
            
            lib::recordAll();
        }

        static void __attribute__((destructor)) ObjectFactory_fini()
        {
            delete ObjectFactory::m_impl;
        }
    }
}
