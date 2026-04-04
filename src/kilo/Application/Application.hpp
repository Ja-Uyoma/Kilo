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

#include "kilo/editor/EditorConfig/EditorConfig.hpp"
#include "kilo/editor/append_buffer/append_buffer.hpp"
#include "kilo/terminal/Window/Window.hpp"

#include <filesystem>
#include <span>

namespace kilo::application {

class application final
{
public:
  ///
  /// \brief Default constructor
  ///
  explicit application() noexcept(false);

  ///
  /// \brief Run the application with the given command-line arguments
  /// \param[in] args The command-line arguments passed to the application
  /// \returns EXIT_SUCCESS on success, and EXIT_FAILURE otherwise
  ///
  static auto main(std::span<char const*> args) -> int;

private:
  kilo::editor::editor_config m_editor_config;
  kilo::editor::append_buffer::append_buffer m_abuf;
  kilo::terminal::window_size::window_size m_winsize;

  ///
  /// \brief Open a file and write its contents to memory
  ///
  /// \param[in] path The path to the file
  /// \throws std::ios_base::failure if the file could not be opened or if there was an error while reading from it
  ///
  void open(std::filesystem::path const& path);

  ///
  /// \brief Run the application
  ///
  void run();
};

}   // namespace kilo::application

#endif
