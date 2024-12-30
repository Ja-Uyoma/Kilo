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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Cursor/Cursor.hpp"
#include "Offset/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "Terminal/window/window.hpp"
#include "TerminalMode/TerminalMode.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Kilo::editor {
class Application
{
public:
  explicit Application() = default;

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
  Terminal::TerminalMode m_mode;

  std::vector<std::string> m_row;
  std::vector<std::string> m_render;
  Cursor m_cursor {};
  Offset m_off {};
  Terminal::Window m_window = Terminal::Window::create();
  [[maybe_unused]] int m_rx {};
  ScreenBuffer m_buffer {};
};
}   // namespace Kilo::editor

#endif
