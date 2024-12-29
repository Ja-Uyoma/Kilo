#include "Application.hpp"

#include "Constants/Constants.hpp"
#include "Editor/Editor.hpp"
#include "Terminal/File.hpp"
#include "Terminal/Terminal.hpp"
#include "Utilities/Utilities.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <system_error>

namespace Kilo::editor {
Config::Config()
{
  try {
    m_mode->setRawMode();
  }
  catch (std::system_error const& err) {
    std::cerr << err.code() << ": " << err.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}

/**
 * @brief Position the cursor within the visible window
 *
 */
void Config::scroll() noexcept
{
  editor::scroll(m_cursor, m_off, m_window);
}

/**
 * @brief Perform a screen refresh
 *
 */
void Config::refreshScreen()
{
  /*
   * Hide the cursor when painting and then move it to the home position
   */

  m_buffer.write(EscapeSequences::HideCursorWhenRepainting).write(EscapeSequences::MoveCursorToHomePosition);

  this->drawRows();
  auto const cursorPos = detail::setExactPositionToMoveCursorTo(m_cursor, m_off);

  terminal::File output;
  m_buffer.write(cursorPos).write(EscapeSequences::ShowTheCursor).flush(output);
}

/**
 * @brief Process the result of calling readKey
 *
 */
void Config::processKeypress()
{
  auto const keyPressed = terminal::readKey();

  if (keyPressed == utilities::ctrlKey('q')) {
    utilities::clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
  }

  using enum utilities::EditorKey;
  auto const key = static_cast<utilities::EditorKey>(keyPressed);

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
void Config::drawRows()
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
bool Config::open(std::filesystem::path const& path)
{
  return editor::open(path, m_row, m_render);
}

}   // namespace Kilo::editor
