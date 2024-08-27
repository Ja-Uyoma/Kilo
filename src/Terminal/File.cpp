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

#include "File.hpp"

#include <cerrno>
#include <system_error>
#include <unistd.h>
#include <utility>

namespace Kilo::terminal {

void File::read()
{
  return;
}

/**
 * @brief Write n bytes of buffer to fd
 *
 * @param fd The file descriptor being written to
 * @param buffer The buffer being written from
 * @param nbytes The number of bytes to be written
 */
void File::write(int fd, void const* buffer, std::size_t nbytes)
{
  if (errno = 0; std::cmp_not_equal(::write(fd, buffer, nbytes), nbytes)) {
    throw std::system_error(errno, std::system_category(), "Could not get cursor position");
  }
}

}   // namespace Kilo::terminal
