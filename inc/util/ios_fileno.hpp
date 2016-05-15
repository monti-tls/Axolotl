#ifndef __AXOLOTL_UTIL_IOS_FILENO_H__
#define __AXOLOTL_UTIL_IOS_FILENO_H__

#include <iosfwd>

// This function is used courtesy of Richard B. Kreckel
// See http://www.ginac.de/~kreckel/fileno/

namespace util
{
    template <typename charT, typename traits>
    
    int ios_fileno(const std::basic_ios<charT, traits>& stream);
}

#endif // __AXOLOTL_UTIL_IOS_FILENO_H__
