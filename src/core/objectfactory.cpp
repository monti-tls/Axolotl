#include "core/objectfactory.hpp"

namespace core
{
    std::map<std::size_t, ObjectFactory::Iface> ObjectFactory::m_ifaces;
    std::map<std::size_t, std::string> ObjectFactory::m_names;
    std::map<std::size_t, std::vector<std::pair<std::string, Object>>> ObjectFactory::m_members;
}
