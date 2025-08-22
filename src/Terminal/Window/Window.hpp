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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "File/File.hpp"

#include <cstdint>

namespace Kilo::Terminal {

struct WindowSize
{
  std::int32_t cols;
  std::int32_t rows;
};

class Window
{
public:
  int32_t cols{};
  int32_t rows{};

  /**
   * \brief Create a Window object
   */
  explicit Window();

  /**
   * \brief Create a Window object with the given dimensions
   * \param[in] windowSize The dimensions of the new Window object
   */
  explicit constexpr Window(WindowSize const& windowSize) noexcept
    : cols(windowSize.cols), rows(windowSize.rows)
  {
  }
};

namespace detail {

/**
 * \brief Get the dimensions of the terminal window
 * \returns The dimensions of the terminal window
 * \throws std::system_error on failure
 */
auto getWindowSize() -> WindowSize;

/**
 * \brief Get the position of the cursor in the terminal window
 * \returns The position of the cursor in the terminal window
 * \throws std::system_error on failure
 */
auto getCursorPosition(IO::FileInterface& file) -> WindowSize;

} // namespace detail

} // namespace Kilo::Terminal

#endif
