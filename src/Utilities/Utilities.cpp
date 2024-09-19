/**
 * MIT License
 * Copyright (c) 2023 Jimmy Givans
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Utilities.hpp"

#include <cerrno>
#include <cstring>
#include <system_error>
#include <unistd.h>

namespace Kilo::utilities {
void clearScreenAndRepositionCursor() noexcept
{
  {
    [[maybe_unused]] auto&& rv = ::write(STDOUT_FILENO, "\x1b[2J", 4);
  }
  {
    [[maybe_unused]] auto&& rv = ::write(STDOUT_FILENO, "\x1b[H", 3);
  }
}

[[nodiscard]] long writeAll(int fd, void const* buf, long count)
{
  long totalWritten {};
  auto const* ptr = static_cast<char const*>(buf);

  while (totalWritten < count) {
    long written = ::write(fd, ptr + totalWritten, count - totalWritten);

    if (written == -1) {
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      else {
        throw std::system_error(errno, std::system_category(), std::strerror(errno));
      }
    }

    if (written == 0) {
      break;
    }

    totalWritten += written;
  }

  return totalWritten;
}
}   // namespace Kilo::utilities
