#ifndef __AXOLOTL_UTIL_IOS_FILENAME_H__
#define __AXOLOTL_UTIL_IOS_FILENAME_H__

#include "util/ios_fileno.hpp"

// This function is used courtesy of Richard B. Kreckel
// See http://www.ginac.de/~kreckel/fileno/

namespace util
{
    template <typename charT, typename traits>
    std::string ios_filename(std::basic_ios<charT, traits> const& stream);
}

#endif // __AXOLOTL_UTIL_IOS_FILENAME_H__
