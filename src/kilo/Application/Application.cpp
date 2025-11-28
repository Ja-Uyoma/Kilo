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
#include "kilo/utilities/Utilities.hpp"
#include <fmt/format.h>
#include <system_error>

#include <cstdlib>
#include <iostream>

namespace kilo::editor {

///
/// \brief Position the cursor within the visible window
///
void application::scroll() noexcept
{
  editor::scroll(m_editor_config);
}

///
/// \brief Perform a screen refresh
///
void application::refresh_screen()
{
  editor::refresh_screen(m_editor_config);
}

///
/// \brief Process the result of calling readKey
///
void application::process_keypress()
{
  auto const key_pressed = io::read_key();
  editor::process_keypress(key_pressed, m_editor_config);
}

///
/// \brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
///
void application::draw_rows()
{
  editor::draw_rows(m_editor_config);
}

///
/// \brief Open a file and write its contents to memory
///
/// \param[in] path The path to the file
/// \return true If the operation was successful, and false otherwise
///
auto application::open(std::filesystem::path const& path) -> bool
{
  return editor::open(path, m_editor_config.open_doc, m_editor_config.rendered_doc);
}

///
/// \brief Run the application
///
void application::run()
try {
  while (true) {
    scroll();
    refresh_screen();
    process_keypress();
  }
}
catch (std::system_error const& err) {
  utilities::clear_screen_and_reposition_cursor();
  std::cerr << err.code() << ": " << err.what() << '\n';
}

}   // namespace kilo::editor
