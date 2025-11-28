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

#include <cstdint>
#include <string>

namespace kilo::io {

class file_interface
{
public:
  ///
  /// \brief Default constructor
  ///
  explicit file_interface() noexcept = default;

  ///
  /// \brief Read all bytes from file_descriptor into buffer
  /// \param[in] file_descriptor The file being read from
  /// \param[in] buffer The buffer being read to
  /// \returns The number of bytes read
  ///
  virtual auto read(int file_descriptor, std::string& buffer) noexcept -> int64_t = 0;

  ///
  /// \brief Read nbytes from file_descriptor into buffer
  /// \param[in] file_descriptor The file being read from
  /// \param[in] buffer The buffer being read to
  /// \param[in] nbytes The number of bytes to read
  /// \returns The number of bytes read
  ///
  virtual auto read(int file_descriptor, std::string& buffer, std::size_t nbytes) -> int64_t = 0;

  ///
  /// \brief Write all bytes of buffer to file_descriptor
  /// \param[in] file_descriptor The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \returns The number of bytes written
  ///
  virtual auto write(int file_descriptor, std::string const& buffer) noexcept -> int64_t = 0;

  ///
  /// \brief Write nbytes of buffer to file_descriptor
  /// \param[in] file_descriptor The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \param[in] nbytes The number of bytes to write
  /// \returns The number of bytes written
  ///
  virtual auto write(int file_descriptor, std::string const& buffer, std::size_t nbytes) -> int64_t = 0;

  ///
  /// \brief Virtual destructor
  ///
  virtual ~file_interface() = default;

  ///
  /// \brief Copy constructor
  ///
  file_interface(file_interface const&) = default;

  ///
  /// \brief Copy-assignment operator
  ///
  auto operator=(file_interface const&) -> file_interface& = default;

  ///
  /// \brief Move constructor
  ///
  file_interface(file_interface&&) = default;

  ///
  /// \brief Move-assignment operator
  ///
  auto operator=(file_interface&&) -> file_interface& = default;
};

class file : public file_interface
{
public:
  ///
  /// \brief Default constructor
  ///
  explicit file() noexcept = default;

  ///
  /// \brief Read all bytes from file_descriptor into buffer
  /// \param[in] file_descriptor The file being read from
  /// \param[in] buffer The buffer being read to
  /// \returns The number of bytes read
  ///
  auto read(int file_descriptor, std::string& buffer) noexcept -> int64_t override;

  ///
  /// \brief Read nbytes from file_descriptor into buffer
  /// \param[in] file_descriptor The file being read from
  /// \param[in] buffer The buffer being read to
  /// \param[in] nbytes The number of bytes to read
  /// \returns The number of bytes read
  ///
  auto read(int file_descriptor, std::string& buffer, std::size_t nbytes) noexcept -> int64_t override;

  ///
  /// \brief Write all bytes of buffer to file_descriptor
  /// \param[in] file_descriptor The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \returns The number of bytes written
  ///
  auto write(int file_descriptor, std::string const& buffer) noexcept -> int64_t override;

  ///
  /// \brief Write nbytes of buffer to file_descriptor
  /// \param[in] file_descriptor The file descriptor being written to
  /// \param[in] buffer The buffer being written from
  /// \param[in] nbytes The number of bytes to write
  /// \returns The number of bytes written
  ///
  auto write(int file_descriptor, std::string const& buffer, std::size_t nbytes) noexcept -> int64_t override;
};

}   // namespace kilo::io

#endif
