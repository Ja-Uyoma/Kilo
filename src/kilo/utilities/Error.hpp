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

#ifndef KILO_UTILITIES_ERROR_HPP
#define KILO_UTILITIES_ERROR_HPP

#include <system_error>

#include <string>
#include <utility>

namespace kilo::utilities {

struct error_info
{
  std::error_code code;
  std::string message;
};

inline auto make_error(int error_number, std::string message) -> error_info
{
  auto const code = error_number == 0 ? std::make_error_code(std::errc::io_error)
                                      : std::error_code(error_number, std::system_category());
  return {.code = code, .message = std::move(message)};
}

inline auto make_error(std::errc error, std::string message) -> error_info
{
  return {.code = std::make_error_code(error), .message = std::move(message)};
}

}   // namespace kilo::utilities

#endif
