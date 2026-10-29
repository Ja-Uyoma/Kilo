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

#include "application.hpp"

#include "kilo/editor/editor_config/editor_config.hpp"
#include "kilo/io/IO.hpp"
#include "kilo/terminal/TerminalMode/TerminalMode.hpp"
#include "kilo/terminal/window_size/window_size.hpp"
#include "kilo/utilities/Utilities.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <span>

namespace kilo::application {

auto application::initialize() -> std::expected<void, utilities::error_info>
{
  auto result = terminal::window_size::get_terminal_window_size();
  if (!result) {
    return std::unexpected(result.error());
  }
  m_winsize = *result;
  m_winsize.rows -= 2;
  return {};
}

auto application::open(std::filesystem::path const& path) -> std::expected<void, utilities::error_info>
{
  return kilo::editor::editor_config::open(path, &m_editor_config);
}

auto application::run() -> std::expected<void, utilities::error_info>
{
  kilo::editor::editor_config::set_status_msg(&m_editor_config, "HELP: Ctrl-Q = quit");

  while (true) {
    auto refresh_result = kilo::editor::editor_config::refresh_screen(m_editor_config, &m_abuf, m_winsize);
    if (!refresh_result) {
      return std::unexpected(refresh_result.error());
    }

    auto key_pressed = io::read_key();
    if (!key_pressed) {
      return std::unexpected(key_pressed.error());
    }

    kilo::editor::editor_config::process_keypress(*key_pressed, m_editor_config, m_winsize);
  }
}

auto application::main(std::span<char const*> args) -> int
{
  static kilo::terminal::terminal_mode term_mode;
  auto terminal_init = term_mode.initialize();
  if (!terminal_init) {
    std::cerr << terminal_init.error().message << ": " << terminal_init.error().code.message() << '\n';
    return EXIT_FAILURE;
  }
  auto raw_mode_result = term_mode.set_raw_mode();
  if (!raw_mode_result) {
    std::cerr << raw_mode_result.error().message << ": " << raw_mode_result.error().code.message() << '\n';
    return EXIT_FAILURE;
  }

  application app;
  auto app_init = app.initialize();
  if (!app_init) {
    kilo::utilities::clear_screen_and_reposition_cursor();
    std::cerr << app_init.error().message << ": " << app_init.error().code.message() << '\n';
    return EXIT_FAILURE;
  }

  if (args.size() >= 2) {
    auto open_result = app.open(args[1]);
    if (!open_result) {
      std::cerr << open_result.error().message << ": " << open_result.error().code.message() << '\n';
      return EXIT_FAILURE;
    }
  }

  auto run_result = app.run();
  if (!run_result) {
    kilo::utilities::clear_screen_and_reposition_cursor();
    std::cerr << run_result.error().message << ": " << run_result.error().code.message() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

}   // namespace kilo::application
