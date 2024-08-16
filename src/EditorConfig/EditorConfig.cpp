#include "EditorConfig.hpp"

#include "Terminal/Terminal.hpp"
#include <iostream>
#include <system_error>

namespace Kilo::editor {
EditorConfig::EditorConfig()
{
  try {
    Terminal::getWindowSize(&window.rows, &window.cols);
    state.setRawMode();
  }
  catch (std::system_error const& err) {
    std::cerr << err.code() << ": " << err.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}

EditorConfig::~EditorConfig()
{
  // We need to call state.reset() at program exit to reset the terminal to its
  // canonical settings We cannot register an atexit handler for this because
  // atexit doesn't accept functions that take parameters. Therefore, the
  // solution is to use RAII with a static object whose resources must be
  // cleaned up at program exit

  state.reset();
}
}   // namespace Kilo::editor
