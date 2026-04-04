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

#include "append_buffer.hpp"

#include "kilo/io/File.hpp"
#include <gsl/assert>
#include <system_error>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <unistd.h>

namespace kilo::editor::append_buffer {

///
/// \brief Flush the buffer by writing its contents to a file
/// \param[in] file The file being written to
/// \returns The number of bytes written
/// \throws `std::system_error` if the operation failed
///
auto append_buffer::flush(io::file_interface& file) const& noexcept(false) -> std::size_t
{
  std::size_t total_written = 0;

  while (total_written < m_buffer.length()) {
    int64_t const result =
      file.write(STDOUT_FILENO, m_buffer.substr(0 + total_written, m_buffer.length() - total_written));

    if (result == -1) {
      if (errno == EINTR or errno == EAGAIN) {
        continue;
      }
      throw std::system_error(errno, std::system_category());
    }

    if (result == 0) {
      break;
    }

    total_written += static_cast<std::size_t>(result);
  }

  Ensures((total_written == m_buffer.length() or total_written == 0) and
          "The total number of bytes written is unequal to the size of the buffer");
  return total_written;
}

}   // namespace kilo::editor::append_buffer
