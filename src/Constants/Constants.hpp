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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string_view>

#include <cstdint>

namespace Kilo::editor {

struct EscapeSequences
{
  static constexpr std::string_view HideCursorWhenRepainting {"\x1b[?25l"};
  static constexpr std::string_view MoveCursorToHomePosition {"\x1b[H"};
  static constexpr std::string_view ShowTheCursor {"\x1b[?25h"};
  static constexpr std::string_view ErasePartOfLineToTheRightOfCursor {"\x1b[K"};
};

// The current version of the application
inline constexpr std::string_view KiloVersion {"0.0.1"};

// The size of a tab character
inline constexpr int KiloTabStop = 8;

// The keys supported by the application
// We choose a representation for the arrow keys that does not conflict with the [w, a, s, d] keys.
// We give them a large integer value that is outside the range of a char, so that they don't
// conflict with ordinary keypresses.
enum class EditorKey : std::uint16_t
{
  ArrowLeft = 1000,
  ArrowRight,
  ArrowUp,
  ArrowDown,
  Delete,
  Home,
  End,
  PageUp,
  PageDown
};

}   // namespace Kilo::editor

#endif
