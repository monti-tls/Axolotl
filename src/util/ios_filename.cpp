#include "util/ios_filename.hpp"

#include <sstream>
#include <cstring>
#include <unistd.h>

namespace util
{
    template <typename charT, typename traits>
    inline std::string filename_hack(std::basic_ios<charT, traits> const& stream)
    {
        std::string path = "???";

        int fd = ios_fileno(stream);
        if (fd >= 0)
        {
            std::ostringstream ss;
            ss << "/proc/self/fd/" << fd;

            char* buffer = new char[512];
            std::memset(buffer, 0, 512);
            if (readlink(ss.str().c_str(), buffer, 512) >= 0)
            {
                path = buffer;
                size_t base = path.find_last_of('/');
                if (base != std::string::npos)
                    path = std::string(path.begin() + base + 1, path.end());
            }
            delete[] buffer;
        }

        return path;
    }

    //! 8-Bit character instantiation: fileno(ios).
    template <>
    std::string ios_filename<char>(const std::ios& stream)
    { return filename_hack(stream); }

    #if !(defined(__GLIBCXX__) || defined(__GLIBCPP__)) || (defined(_GLIBCPP_USE_WCHAR_T) || defined(_GLIBCXX_USE_WCHAR_T))
    //! Wide character instantiation: fileno(wios).
    template <>
    std::string ios_filename<wchar_t>(const std::wios& stream)
    { return filename_hack(stream); }
    #endif
}
