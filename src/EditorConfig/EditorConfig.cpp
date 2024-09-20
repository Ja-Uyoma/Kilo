#include "EditorConfig.hpp"

#include "Editor/Editor.hpp"
#include "Editor/constants/constants.hpp"
#include "Terminal/Terminal.hpp"
#include <iostream>
#include <system_error>

namespace Kilo::editor {
Config::Config()
{
  try {
    m_state.setRawMode();
  }
  catch (std::system_error const& err) {
    std::cerr << err.code() << ": " << err.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}

Config::~Config()
{
  // We need to call state.reset() at program exit to reset the terminal to its
  // canonical settings We cannot register an atexit handler for this because
  // atexit doesn't accept functions that take parameters. Therefore, the
  // solution is to use RAII with a static object whose resources must be
  // cleaned up at program exit

  m_state.reset();
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
  m_buffer.write(cursorPos).write(EscapeSequences::ShowTheCursor).flush();
}

/**
 * @brief Process the result of calling readKey
 *
 */
void Config::processKeypress()
{
  auto const keyPressed = terminal::readKey();

  editor::processKeypress(keyPressed, m_cursor, m_window, m_row);
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
