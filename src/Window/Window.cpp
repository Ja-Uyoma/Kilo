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

#include "Window.hpp"

#include "Terminal/Terminal.hpp"
#include "Terminal/WindowSize.hpp"
#include <iostream>
#include <sys/ioctl.h>
#include <system_error>

namespace Kilo::window {

/**
 * @brief Create a static Window instance and return a reference to it
 *
 * @return Window& A reference to the static Window instance
 */
Window& Window::create()
{
  static Window window;
  return window;
}

Window::Window()
{
  ::winsize ws;

  try {
    auto [c, r] = terminal::getWindowSize(ws);
    m_cols = c;
    m_rows = r;
  }
  catch (std::system_error const& err) {
    std::cerr << err.code() << ": " << err.what() << '\n';
    m_cols = 0;
    m_rows = 0;

    throw;
  }
}

}   // namespace Kilo::window
