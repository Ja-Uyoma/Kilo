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

namespace Kilo::window {

class Window
{
public:
  /**
   * @brief Construct a new Window object
   *
   */
  constexpr explicit Window() noexcept = default;

  /**
   * @brief Create a new Window object from the given row and column dimensions
   *
   * @param[in] rows The number of rows of the Window
   * @param[in] cols The number of columns of the Window
   */
  constexpr explicit Window(int const rows, int const cols) noexcept : m_rows(rows), m_cols(cols)
  {
  }

  /**
   * @brief Get the number of rows of the terminal window
   *
   * @return The number of rows of the terminal window
   */
  constexpr auto rows() const noexcept
  {
    return m_rows;
  }

  /**
   * @brief Get the number of columns of the terminal window
   *
   * @return The number of columns of the terminal window
   */
  constexpr auto cols() const noexcept
  {
    return m_cols;
  }

  int m_rows {};
  int m_cols {};
};

}   // namespace Kilo::window

#endif
