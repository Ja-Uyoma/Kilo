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

#include "kilo/editor/Cursor/Cursor.hpp"
#include "kilo/editor/Offset/Offset.hpp"
#include "kilo/editor/ScreenBuffer/ScreenBuffer.hpp"
#include "kilo/terminal/Window/Window.hpp"

#include <vector>

namespace kilo::editor {

struct editor_config
{
  terminal::window window;
  cursor curs;
  offset off;
  screen_buffer screen_buf;
  std::vector<std::string> open_doc;
  std::vector<std::string> render;
};

}  // namespace kilo::editor

#endif
