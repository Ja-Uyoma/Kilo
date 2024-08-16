#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "Cursor.hpp"
#include "Offset.hpp"
#include "Window.hpp"
#include <string>
#include <vector>
#include "TerminalState/TerminalState.hpp"

namespace Kilo::Editor {
class EditorConfig
{
public:
  TerminalState state {};
  std::vector<std::string> row;
  std::vector<std::string> render;
  int numrows {};
  Cursor cursor {};
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
