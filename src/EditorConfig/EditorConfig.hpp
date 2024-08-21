#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "Editor/Cursor.hpp"
#include "Editor/Offset.hpp"
#include "TerminalState/TerminalState.hpp"
#include "Window/Window.hpp"
#include <string>
#include <vector>

namespace Kilo::editor {
class EditorConfig
{
public:
  terminal::TerminalState state {};
  std::vector<std::string> row;
  std::vector<std::string> render;
  int numrows {};
  Cursor cursor {};
  Offset off {};
  terminal::Window window = terminal::Window::create();
  int rx {};

  explicit EditorConfig();
  ~EditorConfig();

  EditorConfig(EditorConfig const&) = delete;
  EditorConfig& operator=(EditorConfig const&) = delete;

  EditorConfig(EditorConfig&&) noexcept = delete;
  EditorConfig operator=(EditorConfig&&) noexcept = delete;
};
}   // namespace Kilo::editor

#endif
