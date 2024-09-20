#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "Editor/cursor/cursor.hpp"
#include "Editor/offset/offset.hpp"
#include "Editor/screen_buffer/screen_buffer.hpp"
#include "TerminalState/TerminalState.hpp"
#include "Window/Window.hpp"
#include <filesystem>
#include <string>
#include <vector>

namespace Kilo::editor {
class Config
{
public:
  explicit Config();
  ~Config();

  Config(Config const&) = delete;
  Config& operator=(Config const&) = delete;

  Config(Config&&) noexcept = delete;
  Config operator=(Config&&) noexcept = delete;

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

  /**
   * @brief Open a file and write its contents to memory
   *
   * @param[in] path The path to the file
   * @return true If the operation was successful
   * @return false If the operation failed
   */
  bool open(std::filesystem::path const& path);

private:
  terminal::TerminalState m_state {};
  std::vector<std::string> m_row;
  std::vector<std::string> m_render;
  Cursor m_cursor {};
  Offset m_off {};
  terminal::Window m_window = terminal::Window::create();
  [[maybe_unused]] int m_rx {};
  ScreenBuffer m_buffer {};
};
}   // namespace Kilo::editor

#endif
