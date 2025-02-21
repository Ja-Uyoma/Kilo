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

#include "Application.hpp"

#include "Constants/Constants.hpp"
#include "Editor/Editor.hpp"
#include "File/File.hpp"
#include "IO/IO.hpp"
#include "Utilities/Utilities.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace Kilo::editor {

/// Default constructor
Application::Application() noexcept
try : m_mode(), m_window() {
  // Nothing to do here...
}
catch (std::system_error const& err) {
  std::cerr << err.what() << '\n';
  std::exit(EXIT_FAILURE);
}

/**
 * @brief Position the cursor within the visible window
 *
 */
void Application::scroll() noexcept
{
  editor::scroll(m_cursor, m_off, m_window);
}

/**
 * @brief Perform a screen refresh
 *
 */
void Application::refreshScreen()
{
  /*
   * Hide the cursor when painting and then move it to the home position
   */

  m_buffer.write(EscapeSequences::HideCursorWhenRepainting).write(EscapeSequences::MoveCursorToHomePosition);

  this->drawRows();
  auto const cursorPos = detail::setExactPositionToMoveCursorTo(m_cursor, m_off);

  IO::File output;
  m_buffer.write(cursorPos).write(EscapeSequences::ShowTheCursor).flush(output);
}

/**
 * @brief Process the result of calling readKey
 *
 */
void Application::processKeypress()
{
  auto const keyPressed = IO::readKey();

  if (keyPressed == utilities::ctrlKey('q')) {
    utilities::clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
  }

  using enum editor::EditorKey;
  auto const key = static_cast<editor::EditorKey>(keyPressed);

  if (key == Home) {
    m_cursor.x = 0;
  }
  else if (key == End) {
    m_cursor.x = m_window.cols() - 1;
  }
  else if (key == PageUp or key == PageDown) {
    for (std::size_t i = m_window.rows(); i > 0; --i) {
      moveCursor(key == PageUp ? ArrowUp : ArrowDown, m_cursor, m_row);
    }
  }
  else if (key == ArrowLeft or key == ArrowRight or key == ArrowUp or key == ArrowDown) {
    moveCursor(key, m_cursor, m_row);
  }
}

/**
 * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
 */
void Application::drawRows()
{
  editor::drawRows(m_window, m_off, m_row, m_buffer, m_render);
}

/**
 * @brief Open a file and write its contents to memory
 *
 * @param[in] path The path to the file
 * @return true If the operation was successful
 * @return false If the operation failed
 */
bool Application::open(std::filesystem::path const& path)
{
  return editor::open(path, m_row, m_render);
}

}   // namespace Kilo::editor
