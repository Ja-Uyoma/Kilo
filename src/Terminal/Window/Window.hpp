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

namespace Kilo::Terminal {

struct WindowSize
{
  int cols;
  int rows;
};

class Window
{
public:
  /// Create a new Window object
  explicit Window();

  /// Get the number of columns of the terminal window
  /// \returns The number of columns of the terminal window
  constexpr auto cols() const noexcept
  {
    return m_winsize.cols;
  }

  /// Get the number of rows of the terminal window
  /// \returns The number of rows of the terminal window
  constexpr auto rows() const noexcept
  {
    return m_winsize.rows;
  }

private:
  WindowSize m_winsize;
};

namespace detail {

/// Get the size of the open terminal window
/// \throws std::system_error on failure
/// \returns The size of the terminal window as a WindowSize instance on success
WindowSize getWindowSize();

/// Get the position of the cursor in the terminal window
/// \throws std::system_error on failure
/// \returns The position of the cursor as a WindowSize instance
WindowSize getCursorPosition(IO::FileInterface& file);

}   // namespace detail

}   // namespace Kilo::Terminal

#endif
