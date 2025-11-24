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

/// Default constructor
Application::Application() noexcept = default;

/**
 * @brief Position the cursor within the visible window
 *
 */
void Application::scroll() noexcept
{
  editor::scroll(editorConfig);
}

/**
 * @brief Perform a screen refresh
 *
 */
void Application::refreshScreen()
{
  editor::refreshScreen(editorConfig);
}

/**
 * @brief Process the result of calling readKey
 *
 */
void Application::processKeypress()
{
  auto const keyPressed = io::readKey();
  editor::processKeypress(keyPressed, editorConfig);
}

/**
 * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
 */
void Application::drawRows()
{
  editor::drawRows(editorConfig);
}

/**
 * @brief Open a file and write its contents to memory
 *
 * @param[in] path The path to the file
 * @return true If the operation was successful
 * @return false If the operation failed
 */
auto Application::open(std::filesystem::path const& path) -> bool
{
  return editor::open(path, editorConfig.openDoc, editorConfig.renderedDoc);
}

void Application::run()
try {
  while (true) {
    scroll();
    refreshScreen();
    processKeypress();
  }
}
catch (std::system_error const& err) {
  utilities::clearScreenAndRepositionCursor();
  std::cerr << err.code() << ": " << err.what() << '\n';
}

}   // namespace kilo::editor
