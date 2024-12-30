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

#include "TerminalMode.hpp"

#include "Termios/Termios.hpp"
#include <cassert>
#include <cerrno>
#include <termios.h>
#include <unistd.h>

namespace Kilo::Terminal {

TerminalMode::TerminalMode()
{
  getTerminalDriverSettings(STDIN_FILENO, m_termios);
  setRawMode();
}

void TerminalMode::setRawMode() &
{
  if (m_state == ttystate::Raw) {
    return;
  }

  assert(m_state == ttystate::Reset && "Terminal driver currently in canonical mode");
  ttyRaw(STDIN_FILENO, m_termios, m_copy);
  m_state = ttystate::Raw;
}

void TerminalMode::reset() &
{
  if (m_state == ttystate::Reset) {
    return;
  }

  assert(m_state == ttystate::Raw && "Terminal driver currently in raw mode");
  ttyReset(STDIN_FILENO, m_termios);
  m_state = ttystate::Reset;
}

}   // namespace Kilo::Terminal
