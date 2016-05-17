#include "lib/lib.hpp"

namespace lib
{
    void recordAll()
    {
        Core::record();
        Lang::record();
        Dict::record();
    }
}
