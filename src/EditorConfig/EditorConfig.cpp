#include "EditorConfig.hpp"

#include "Editor/Constants.hpp"
#include "Editor/Editor.hpp"
#include "Terminal/Terminal.hpp"
#include <iostream>
#include <system_error>

namespace Kilo::editor {
EditorConfig::EditorConfig()
{
  try {
    state.setRawMode();
  }
  catch (std::system_error const& err) {
    std::cerr << err.code() << ": " << err.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}

EditorConfig::~EditorConfig()
{
  // We need to call state.reset() at program exit to reset the terminal to its
  // canonical settings We cannot register an atexit handler for this because
  // atexit doesn't accept functions that take parameters. Therefore, the
  // solution is to use RAII with a static object whose resources must be
  // cleaned up at program exit

  state.reset();
}

/**
 * @brief Position the cursor within the visible window
 *
 */
void EditorConfig::scroll() noexcept
{
  editor::scroll(cursor, off, window);
}

/**
 * @brief Perform a screen refresh
 *
 */
void EditorConfig::refreshScreen()
{
  /*
   * Hide the cursor when painting and then move it to the home position
   */

  buffer.write(EscapeSequences::HideCursorWhenRepainting).write(EscapeSequences::MoveCursorToHomePosition);

  this->drawRows();
  auto const cursorPos = detail::setExactPositionToMoveCursorTo(cursor, off);
  buffer.write(cursorPos).write(EscapeSequences::ShowTheCursor).flush();
}

/**
 * @brief Process the result of calling readKey
 *
 */
void EditorConfig::processKeypress()
{
  auto const keyPressed = terminal::readKey();

  editor::processKeypress(keyPressed, cursor, window);
}

/**
 * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
 */
void EditorConfig::drawRows()
{
  editor::drawRows(window, off, row, buffer, render);
}

}   // namespace Kilo::editor
