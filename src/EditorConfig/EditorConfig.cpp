#include "EditorConfig.hpp"

#include "Editor/Editor.hpp"
#include <iostream>
#include <system_error>

namespace Kilo::editor {
EditorConfig::EditorConfig()
{
  try {
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

/**
 * @brief Position the cursor within the visible window
 *
 */
void EditorConfig::scroll() noexcept
{
  editor::scroll(cursor, off, window);
}
}   // namespace Kilo::editor
