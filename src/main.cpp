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

#include "kilo/Application/Application.hpp"
#include "kilo/terminal/TerminalMode/TerminalMode.hpp"

#include <cstdlib>
#include <iostream>

using kilo::editor::application;
using kilo::terminal::terminal_mode;

auto main(int argc, char const* argv[]) -> int
{
  try {
    static terminal_mode terminal_mode;
    terminal_mode.set_raw_mode();
  }
  catch (std::system_error const& err) {
    std::cerr << err.code().message() << ": " << err.what() << '\n';
    return EXIT_FAILURE;
  }

  application app;

  if (argc >= 2 && !app.open(argv[1])) {
    return EXIT_FAILURE;
  }

  app.run();

  return EXIT_SUCCESS;
}
