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

#ifndef FILE_HPP
#define FILE_HPP

#include <cstddef>
#include <string>

namespace Kilo::IO {

class FileInterface
{
public:
  /// Read all bytes from fd into buffer
  /// \param[in] fd The file being read from
  /// \param[in] buffer The buffer being read to
  /// \returns The number of bytes read
  virtual auto read(int fd, std::string& buffer) noexcept -> std::size_t = 0;

  /// Read nbytes from fd into buffer
  /// \param[in] fd The file being read from
  /// \param[in] buffer The buffer being read to
  /// \param[in] nbytes The number of bytes to read
  /// \returns The number of bytes read
  virtual auto read(int fd, std::string& buffer, std::size_t nbytes) -> std::size_t = 0;

  /// Write all bytes of buffer to fd
  /// \param[in] fd The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \returns The number of bytes written
  virtual auto write(int fd, std::string const& buffer) noexcept -> std::size_t = 0;

  /// Write nbytes of buffer to fd
  /// \param[in] fd The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \param[in] nbytes The number of bytes to write
  /// \returns The number of bytes written
  virtual auto write(int fd, std::string const& buffer, std::size_t nbytes) -> std::size_t = 0;

  /// Virtual destructor
  virtual ~FileInterface() = default;
};

class File : public FileInterface
{
public:
  /// Default constructor
  explicit File() noexcept = default;

  /// Read all bytes from fd into buffer
  /// \param[in] fd The file being read from
  /// \param[in] buffer The buffer being read to
  /// \returns The number of bytes read
  auto read(int fd, std::string& buffer) noexcept -> std::size_t override;

  /// Read nbytes from fd into buffer
  /// \param[in] fd The file being read from
  /// \param[in] buffer The buffer being read to
  /// \param[in] nbytes The number of bytes to read
  /// \returns The number of bytes read
  auto read(int fd, std::string& buffer, std::size_t nbytes) noexcept -> std::size_t override;

  /// Write all bytes of buffer to fd
  /// \param[in] fd The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \returns The number of bytes written
  auto write(int fd, std::string const& buffer) noexcept -> std::size_t override;

  /// Write nbytes of buffer to fd
  /// \param[in] fd The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \param[in] nbytes The number of bytes to write
  /// \returns The number of bytes written
  auto write(int fd, std::string const& buffer, std::size_t nbytes) noexcept -> std::size_t override;
};

}   // namespace Kilo::IO

#endif
