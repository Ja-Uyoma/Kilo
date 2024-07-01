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

#include <cstddef>

namespace Kilo::Utilities {
/// @brief The version number of the application
inline constexpr char const* KILO_VERSION = "0.0.1";

// We choose a representation for arrow keys that doesn't conflict with w, a, s,
// d. We give them a large integer value that is out of the range of a char, so
// that they don't conflict with ordinary keypresses.
enum class EditorKey : int
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

/// @brief Clear the screen and reposition the cursor to the top-left corner
void clearScreenAndRepositionCursor() noexcept;

/// @brief Map characters to control keys
/// @param key The ASCII character to be mapped to a control key
/// @return A control key
constexpr unsigned ctrlKey(unsigned char key) noexcept
{
  key &= 0x1f;
  return key;
}

/**
 * @brief Write to a file descriptor, with retries in case of partial writes
 * @param[in] fd The file descriptor to write to
 * @param[in] buf The buffer being written from
 * @param[in] count The number of bytes to be written
 * @returns The total number of bytes written
 * @throws std::system_error in case of total write failure
 */
[[nodiscard]] long writeAll(int fd, void const* buf, long count);
}   // namespace Kilo::Utilities
