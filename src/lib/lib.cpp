#include "lib/lib.hpp"

namespace lib
{
    void recordAll()
    {
        Core::record();
        Scalars::record();
        Lang::record();
        Dict::record();
    }
}
