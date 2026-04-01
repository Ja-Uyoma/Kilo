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

#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "kilo/editor/ScreenBuffer/ScreenBuffer.hpp"
#include "kilo/terminal/Window/Window.hpp"

#include <vector>

namespace kilo::editor {

struct cursor
{
  std::int64_t x {};
  std::int64_t y {};
};

struct offset
{
  std::int64_t row {};
  std::int64_t col {};
};

/**
 * @struct erow
 */
struct erow
{
  std::string chars;
  std::string render;

  /**
   * @brief Create an erow instance from the given std::string object
   * @param[in] line The std::string from which the erow is to be created
   */
  explicit erow(std::string line) : chars(std::move(line))
  {
  }
};

struct editor_config
{
  terminal::window_size winsize;
  cursor curs;
  offset off;
  screen_buffer screen_buf;
  std::vector<erow> row;
};

}   // namespace kilo::editor

#endif
