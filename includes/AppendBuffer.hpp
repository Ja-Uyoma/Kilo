#ifndef APPEND_BUFFER_HPP
#define APPEND_BUFFER_HPP

#include <string>

namespace Kilo::AppendBuffer
{
    // In order to avoid making multiple ::write() calls anytime we need to refresh the screen,
    // we will do one big ::write() at the end to make sure the entire screen updates at once.
    // This is accomplished by the use of a buffer to which strings will be appended, and then
    // this buffer will be written out at the end.
    struct AppendBuffer
    {
        std::string buffer;
    };
}

#endif