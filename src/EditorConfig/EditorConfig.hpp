#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "Cursor/Cursor.hpp"
#include "Offset/Offset.hpp"
#include "TerminalState/TerminalState.hpp"
#include "Window/Window.hpp"
#include <string>
#include <vector>

namespace Kilo::Editor {
class EditorConfig
{
public:
  TerminalState state {};
  std::vector<std::string> row;
  std::vector<std::string> render;
  int numrows {};
  cursor::Cursor cursor {};
  Offset off {};
  Window window {};
  int rx {};

  explicit EditorConfig();
  ~EditorConfig();

  EditorConfig(EditorConfig const&) = delete;
  EditorConfig& operator=(EditorConfig const&) = delete;

  EditorConfig(EditorConfig&&) noexcept = delete;
  EditorConfig operator=(EditorConfig&&) noexcept = delete;
};
}   // namespace Kilo::Editor

#endif
