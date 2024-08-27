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

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <cstddef>
namespace Kilo::terminal {

class System
{
public:
  /**
   * @brief Read nbytes from fd into buffer
   *
   * @param fd The file being read from
   * @param buffer The buffer being read to
   * @param nbytes The number of bytes to read
   */
  virtual void read(int fd, void* buffer, std::size_t nbytes);

  /**
   * @brief Write n bytes of buffer to fd
   *
   * @param fd The file descriptor being written to
   * @param buffer The buffer being written from
   * @param nbytes The number of bytes to be written
   */
  virtual void write(int fd, void const* buffer, std::size_t nbytes);
  virtual ~System();
};

}   // namespace Kilo::terminal

#endif
