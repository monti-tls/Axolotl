#include "lib/lib.hpp"

namespace lib
{
    void recordAll()
    {
        Core::record();
        Io::record();
        Lang::record();
        Dict::record();
    }
}
