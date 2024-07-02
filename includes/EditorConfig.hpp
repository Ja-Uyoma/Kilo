#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include <string>
#include <termios.h>
#include <vector>

namespace Kilo::Editor {
class EditorConfig
{
public:
  termios origTermios;
  int screenRows;
  int screenCols;
  int cursorX {};   // The horizontal coordinate of the cursor (the column)
  int cursorY {};   // The vertical coordinate of the cursor (the row)
  std::vector<std::string> row;
  std::vector<std::string> render;
  int numrows {};
  int rowoff {};
  int coloff {};

  explicit EditorConfig();
  ~EditorConfig();

  EditorConfig(EditorConfig const&) = delete;
  EditorConfig& operator=(EditorConfig const&) = delete;

  EditorConfig(EditorConfig&&) noexcept = delete;
  EditorConfig operator=(EditorConfig&&) noexcept = delete;
};
}   // namespace Kilo::Editor

#endif
