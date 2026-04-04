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

#include "erow.hpp"

#include "kilo/utilities/Constants.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>

namespace kilo::editor::erow {

auto row_cx_to_rx(erow const& row, int64_t cursor_x) -> int64_t
{
  using utilities::kilo_tab_stop;

  int64_t render_x {};

  // Loop through all the characters to the left of `cursor_x`, and figure out how many spaces each tab takes up.
  // For each character, if it's a tab we use rx % kilo_tab_stop to find out how many columns we are to the right of the
  // last tab stop, then subtract that from kilo_tab_stop - 1 to find out how many columns we are to the left of the
  // next tab stop.
  // We add that amount to render_x to get just to the left of the next tab stop, and then the unconditional ++render_x
  // statement gets us right on the next tab stop. This works even if we are currently on a tab stop.

  for (std::size_t j = 0; std::cmp_less(j, cursor_x); ++j) {
    if (row.chars[j] == '\t') {
      render_x += (kilo_tab_stop - 1) - (render_x % kilo_tab_stop);
    }

    ++render_x;
  }

  return render_x;
}

}   // namespace kilo::editor::erow