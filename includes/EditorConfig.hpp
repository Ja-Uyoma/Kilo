#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include "Cursor.hpp"
#include "Offset.hpp"
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
  std::vector<std::string> row;
  std::vector<std::string> render;
  int numrows {};
  int rowoff {};
  int coloff {};
  Cursor cursor {};
  Offset off {};

  explicit EditorConfig();
  ~EditorConfig();

  EditorConfig(EditorConfig const&) = delete;
  EditorConfig& operator=(EditorConfig const&) = delete;

  EditorConfig(EditorConfig&&) noexcept = delete;
  EditorConfig operator=(EditorConfig&&) noexcept = delete;
};
}   // namespace Kilo::Editor

#endif
