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

#include "ScreenBuffer.hpp"

#include <cassert>
#include <cerrno>
#include <system_error>
#include <unistd.h>

namespace Kilo::editor {

/// \brief Flush the buffer by writing its contents to a file
/// \param[in] file The file being written to
/// \returns The number of bytes written
/// \throws `std::system_error` if the operation failed
std::size_t ScreenBuffer::flush(IO::FileInterface& file) const
{
  std::size_t totalWritten = 0;

  while (totalWritten < m_buffer.length()) {
    errno = 0;
    long result = file.write(STDOUT_FILENO, m_buffer.substr(0 + totalWritten, m_buffer.length() - totalWritten));

    if (result == -1) {
      if (errno == EINTR or errno == EAGAIN) {
        continue;
      }
      else {
        throw std::system_error(errno, std::system_category());
      }
    }

    if (result == 0) {
      break;
    }

    totalWritten += result;
  }

  assert(totalWritten == m_buffer.length() && "The total number of bytes written is unequal to the size of the buffer");
  return totalWritten;
}

}   // namespace Kilo::editor
