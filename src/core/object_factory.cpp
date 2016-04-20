#include "core/object_factory.hpp"

namespace core
{
    std::map<std::size_t, ObjectFactory::Interface> ObjectFactory::m_interfaces;
    std::map<std::size_t, std::string> ObjectFactory::m_names;
    std::map<std::size_t, ObjectFactory::ObjectList> ObjectFactory::m_constructors;
    std::map<std::size_t, ObjectFactory::NamedObjectList> ObjectFactory::m_methods;
}
