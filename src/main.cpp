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

#include "Editor/config/config.hpp"
#include "Utilities/Utilities.hpp"
#include <cstdlib>
#include <iostream>
#include <system_error>

using namespace Kilo;

/**
 * @brief Handles the processing of keypresses and repainting the screen on
 * every refresh
 *
 */
[[noreturn]] void Main(editor::Config& app) noexcept
{
  while (true) {
    try {
      app.scroll();
      app.refreshScreen();
      app.processKeypress();
    }
    catch (std::system_error const& e) {
      /*
       * Clear the screen and reset the cursor as a fallback in case an error
       * occurs in the middle of rendering the screen. We would otherwise have
       * garbage and/or errors printed wherever the cursor happens to be.
       */
      utilities::clearScreenAndRepositionCursor();
      std::cerr << e.code() << ": " << e.what() << '\n';
    }
  }
}

int main(int argc, char const* argv[])
{
  static editor::Config app;

  if (argc >= 2 && !app.open(argv[1])) {
    return EXIT_FAILURE;
  }

  Main(app);

  return EXIT_SUCCESS;
}
