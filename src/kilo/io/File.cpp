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

#include <sys/ioctl.h>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unistd.h>

namespace kilo::io {

auto file::read(int file_descriptor, std::string& buffer) noexcept -> int64_t
{
  return ::read(file_descriptor, buffer.data(), buffer.length());
}

auto file::read(int file_descriptor, std::string& buffer, std::size_t nbytes) noexcept -> int64_t
{
  return ::read(file_descriptor, buffer.data(), nbytes);
}

auto file::read(int file_descriptor, char buffer) noexcept -> int64_t
{
  return ::read(file_descriptor, &buffer, 1);
}

auto file::write(int file_descriptor, std::string const& buffer) noexcept -> int64_t
{
  return ::write(file_descriptor, buffer.c_str(), buffer.length());
}

auto file::write(int file_descriptor, std::string const& buffer, std::size_t nbytes) noexcept -> int64_t
{
  return ::write(file_descriptor, buffer.c_str(), nbytes);
}

auto file::write(int file_descriptor, char buffer) noexcept -> int64_t
{
  return ::write(file_descriptor, &buffer, 1);
}

auto file::ioctl(int file_descriptor, uint64_t request, void* args) noexcept -> int64_t
{
  // NOLINTNEXTLINE(*-vararg)
  return ::ioctl(file_descriptor, request, args);
}

}   // namespace kilo::io
