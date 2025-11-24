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
#include <unistd.h>

namespace Kilo::IO {

/// Read all bytes from fileDescriptor into buffer
/// \param[in] fileDescriptor The file being read from
/// \param[in] buffer The buffer being read to
/// \returns The number of bytes read
std::size_t File::read(int fileDescriptor, std::string& buffer) noexcept
{
  return ::read(fileDescriptor, &buffer[0], buffer.length());
}

/// Read nbytes from fileDescriptor into buffer
/// \param[in] fileDescriptor The file being read from
/// \param[in] buffer The buffer being read to
/// \param[in] nbytes The number of bytes to read
/// \returns The number of bytes read
std::size_t File::read(int fileDescriptor, std::string& buffer, std::size_t nbytes) noexcept
{
  return ::read(fileDescriptor, &buffer[0], nbytes);
}

/// Write all bytes of buffer to fileDescriptor
/// \param[in] fileDescriptor The file descriptor being written to
/// \param[in] buffer The buffer being written from
/// \returns The number of bytes written
std::size_t File::write(int fileDescriptor, std::string const& buffer) noexcept
{
  return ::write(fileDescriptor, buffer.c_str(), buffer.length());
}

/// Write nbytes of buffer to fileDescriptor
/// \param[in] fileDescriptor The file descriptor being written to
/// \param[in] buffer The buffer being written from
/// \param[in] nbytes The number of bytes to write
/// \returns The number of bytes written
std::size_t File::write(int fileDescriptor, std::string const& buffer, std::size_t nbytes) noexcept
{
  return ::write(fileDescriptor, buffer.c_str(), nbytes);
}

}   // namespace Kilo::IO
