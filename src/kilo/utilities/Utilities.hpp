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

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

namespace kilo::utilities {

///
/// \brief Clear the screen and reposition the cursor to the top-left corner
///
void clear_screen_and_reposition_cursor() noexcept;

///
/// \brief Map characters to control keys
/// This function performs a bitwise AND operation to mask the lower 5 bits of the input character while zeroing out all
/// higher bits, thus isolating only the least significant 5 bits.
/// \param[in] key The ASCII character to be mapped to a control key
/// \pre The input must be a printable ASCII character
/// \return A control key
///
constexpr auto ctrl_key(unsigned char key) noexcept -> unsigned
{
  constexpr unsigned char control_key_mask = 0x1f;

  // This is the Unix terminal convention for control key sequences
  // When you press Ctrl + A, for example, the terminal sends the ASCII character with its upper 3 bits cleared
  // A = 0x41, Ctrl+A = 0x01

  key &= control_key_mask;
  return key;
}

}   // namespace kilo::utilities

#endif
