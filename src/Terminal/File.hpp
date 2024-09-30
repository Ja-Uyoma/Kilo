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

namespace Kilo::terminal {

class FileInterface
{
public:
  /**
   * @brief Read nbytes from fd into buffer
   *
   * @param fd The file being read from
   * @param buffer The buffer being read to
   * @param nbytes The number of bytes to read
   * @returns The number of bytes read
   */
  virtual std::size_t read(int fd, std::string& buffer, std::size_t nbytes) = 0;

  /**
   * @brief Write n bytes of buffer to fd
   *
   * @param fd The file descriptor being written to
   * @param buffer The buffer being written from
   * @param nbytes The number of bytes to be written
   * @returns The number of bytes written
   */
  virtual std::size_t write(int fd, std::string const& buffer, std::size_t nbytes) = 0;

  /**
   * @brief Destructor
   *
   */
  virtual ~FileInterface() = default;
};

class File : public FileInterface
{
public:
  explicit File() noexcept = default;

  /**
   * @brief Read nbytes from fd into buffer
   *
   * @param fd The file being read from
   * @param buffer The buffer being read to
   * @param nbytes The number of bytes to read
   * @throws std::system_error On read failure
   * @returns The number of bytes read
   */
  std::size_t read(int fd, std::string& buffer, std::size_t nbytes) override;

  /**
   * @brief Write n bytes of buffer to fd
   *
   * @param fd The file descriptor being written to
   * @param buffer The buffer being written from
   * @param nbytes The number of bytes to be written
   * @throws std::system_error On write failure
   * @returns The number of bytes written
   */
  std::size_t write(int fd, std::string const& buffer, std::size_t nbytes) override;
};

}   // namespace Kilo::terminal

#endif
