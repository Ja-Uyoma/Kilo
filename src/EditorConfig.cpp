#include "EditorConfig.hpp"

#include "Terminal.hpp"
#include <iostream>
#include <system_error>

namespace Kilo::Editor {
EditorConfig::EditorConfig()
{
  try {
    Terminal::enableRawMode(origTermios);
    Terminal::getWindowSize(&screenRows, &screenCols);
  }
  catch (std::system_error const& err) {
    std::cerr << err.code() << ": " << err.what() << '\n';
  }
}

EditorConfig::~EditorConfig()
{
  // We need to call disableRawMode at program exit to reset the terminal to its
  // canonical settings We cannot register an atexit handler for this because
  // atexit doesn't accept functions that take parameters. Therefore, the
  // solution is to use RAII with a static object whose resources must be
  // cleaned up at program exit

  Terminal::disableRawMode(origTermios);
}
}   // namespace Kilo::Editor
