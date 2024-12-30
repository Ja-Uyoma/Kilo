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
#include <string>
#include <system_error>
#include <unistd.h>

namespace Kilo::IO {

/**
 * @brief Read nbytes from fd into buffer
 *
 * @param fd The file being read from
 * @param buffer The buffer being read to
 * @throws std::system_error On read failure
 * @returns The number of bytes read
 */
std::size_t File::read(int fd, std::string& buffer) noexcept
{
  return ::read(fd, &buffer[0], buffer.length());
}

/**
 * @brief Write n bytes of buffer to fd
 *
 * @param fd The file descriptor being written to
 * @param buffer The buffer being written from
 * @returns The number of bytes written
 */
std::size_t File::write(int fd, std::string const& buffer) noexcept
{
  return ::write(fd, buffer.c_str(), buffer.length());
}

}   // namespace Kilo::IO
