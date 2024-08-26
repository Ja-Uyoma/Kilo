#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "Editor/Cursor.hpp"
#include "Editor/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "TerminalState/TerminalState.hpp"
#include "Window/Window.hpp"
#include <string>
#include <vector>

namespace Kilo::editor {
class EditorConfig
{
public:
  terminal::TerminalState state {};
  std::vector<std::string> row;
  std::vector<std::string> render;
  Cursor cursor {};
  Offset off {};
  terminal::Window window = terminal::Window::create();
  int rx {};
  ScreenBuffer buffer {};

  explicit EditorConfig();
  ~EditorConfig();

  EditorConfig(EditorConfig const&) = delete;
  EditorConfig& operator=(EditorConfig const&) = delete;

  EditorConfig(EditorConfig&&) noexcept = delete;
  EditorConfig operator=(EditorConfig&&) noexcept = delete;

  /**
   * @brief Position the cursor within the visible window
   *
   */
  void scroll() noexcept;

  /**
   * @brief Perform a screen refresh
   *
   */
  void refreshScreen();

  /**
   * @brief Process the result of calling readKey
   *
   */
  void processKeypress();

  /**
   * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
   */
  void drawRows();
};
}   // namespace Kilo::editor

#endif
