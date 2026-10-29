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

#ifndef KILO_APPLICATION_APPLICATION_HPP
#define KILO_APPLICATION_APPLICATION_HPP

#include "kilo/editor/append_buffer/append_buffer.hpp"
#include "kilo/editor/editor_config/editor_config.hpp"
#include "kilo/terminal/window_size/window_size.hpp"
#include "kilo/utilities/Error.hpp"

#include <expected>
#include <filesystem>
#include <span>

namespace kilo::application {

class application final
{
public:
  /**
   * @brief Default constructor
   */
  explicit application() noexcept = default;

  /**
   * @brief Determine the terminal dimensions before starting the editor
   * @returns An error if the terminal dimensions cannot be determined
   */
  auto initialize() -> std::expected<void, utilities::error_info>;

  /**
   * @brief Run the application with the given command-line arguments
   * @param[in] args The command-line arguments passed to the application
   * @returns EXIT_SUCCESS on success, and EXIT_FAILURE otherwise
   */
  static auto main(std::span<char const*> args) -> int;

private:
  kilo::editor::editor_config::editor_config m_editor_config;
  kilo::editor::append_buffer::append_buffer m_abuf;
  kilo::terminal::window_size::window_size m_winsize;

  /**
   * @brief Open a file and write its contents to memory
   * @param[in] path The path to the file
   * @returns An error if the file could not be opened or read
   */
  auto open(std::filesystem::path const& path) -> std::expected<void, utilities::error_info>;

  /**
   * @brief Run the application
   * @returns An error if reading input or refreshing the screen fails
   */
  auto run() -> std::expected<void, utilities::error_info>;
};

}   // namespace kilo::application

#endif
