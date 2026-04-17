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

#ifndef KILO_EDITOR_EROW_EROW_HPP
#define KILO_EDITOR_EROW_EROW_HPP

#include <string>

namespace kilo::editor::erow {

/**
 * @class erow
 * @brief Represents a single row of text in the editor
 */
class erow
{
public:
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

/**
 * @brief Copies the contents of the source string into the destination string
 * @param[in] row The source string
 * @param[in] render The destination string
 */
void update_row(std::string_view row, std::string& render) noexcept;

/**
 * @brief Converts a @code chars index into a @code render index
 * @param[in] row A row of text in the editor
 * @param[in] cursor_x The x-position of the cursor in the erow's @code chars field
 * @returns The x-position of the cursor in the erow's @code render field
 */
auto row_cx_to_rx(erow const& row, int64_t cursor_x) -> int64_t;

}   // namespace kilo::editor::erow

#endif