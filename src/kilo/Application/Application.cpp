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

#include "kilo/editor/Editor.hpp"
#include "kilo/io/IO.hpp"
#include "kilo/terminal/TerminalMode/TerminalMode.hpp"
#include "kilo/terminal/Window/Window.hpp"
#include "kilo/utilities/Utilities.hpp"
#include <system_error>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <span>

namespace kilo::application {

application::application() noexcept(false)
{
  m_editor_config.winsize = terminal::get_terminal_window_size();
  m_editor_config.winsize.rows -= 2;
}

void application::open(std::filesystem::path const& path)
{
  kilo::editor::open(path, &m_editor_config);
}

void application::run()
{
  kilo::editor::set_status_msg(&m_editor_config, "HELP: Ctrl-Q = quit");

  try {
    while (true) {
      kilo::editor::refresh_screen(m_editor_config, &m_abuf);

      auto const key_pressed = io::read_key();
      kilo::editor::process_keypress(key_pressed, m_editor_config);
    }
  }
  catch (std::system_error const& err) {
    kilo::utilities::clear_screen_and_reposition_cursor();
    std::cerr << err.code() << ": " << err.what() << '\n';
  }
}

auto application::main(std::span<char const*> args) -> int
{
  try {
    static kilo::terminal::terminal_mode term_mode;
    term_mode.set_raw_mode();
  }
  catch (std::system_error const& err) {
    std::cerr << err.code().message() << ": " << err.what() << '\n';
    return EXIT_FAILURE;
  }

  application app;

  if (args.size() >= 2) {
    try {
      app.open(args[1]);
    }
    catch (std::ios_base::failure const& err) {
      std::cerr << err.what() << '\n' << err.code() << ": " << err.code().message() << '\n';
      return EXIT_FAILURE;
    }
  }

  app.run();

  return EXIT_SUCCESS;
}

}   // namespace kilo::application
