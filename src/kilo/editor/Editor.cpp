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

#include "Editor.hpp"

#include "kilo/io/File.hpp"
#include "kilo/utilities/Constants.hpp"
#include "kilo/utilities/Utilities.hpp"
#include <fmt/format.h>
#include <gsl/assert>
#include <gsl/pointers>
#include <string_view>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

namespace kilo::editor {

/**
 * \brief Move the cursor in the open document depending on the key pressed
 * \param[in] keyPressed The key pressed by the user
 * \param[in] editor The current state of the editor
 */
void process_keypress(int key_pressed, editor_config& editor_config)
{
  std::ignore = std::atexit([]() -> void { utilities::clear_screen_and_reposition_cursor(); });

  if (key_pressed == utilities::ctrl_key('q')) {
    std::exit(EXIT_SUCCESS);
  }

  using enum utilities::editor_key;

  if (auto const key = static_cast<utilities::editor_key>(key_pressed); key == home) {
    editor_config.curs.x = 0;
  }
  else if (key == end) {
    if (editor_config.curs.y < std::ssize(editor_config.row)) {
      editor_config.curs.x = std::ssize(editor_config.row[static_cast<std::size_t>(editor_config.curs.y)].chars);
    }
  }
  else if (key == page_up or key == page_down) {
    if (key == page_up) {
      editor_config.curs.y = editor_config.off.row;
    }
    else if (key == page_down) {
      editor_config.curs.y = editor_config.off.row + editor_config.winsize.rows - 1;
      editor_config.curs.y = std::min(editor_config.curs.y, std::ssize(editor_config.row));
    }

    for (int32_t i = editor_config.winsize.rows; i > 0; --i) {
      move_cursor(key == page_up ? arrow_up : arrow_down, editor_config);
    }
  }
  else if (key == arrow_left or key == arrow_right or key == arrow_up or key == arrow_down) {
    move_cursor(key, editor_config);
  }
}

/**
 * \brief Perform a screen refresh
 * \param[in] editor The current editor configuration
 */
void refresh_screen(editor_config& editor)
{
  scroll(editor);

  // Hide the cursor when painting and then move it to the Home position
  editor.screen_buf.write(utilities::escape_sequences::hide_cursor_when_repainting)
    .write(utilities::escape_sequences::move_cursor_to_home_position);

  // Draw the welcome message, or each row of the currently open document with a tilde at the beginning
  draw_rows(editor);
  draw_status_bar(&editor);

  // We want to show the cursor immediately after writing the contents of the open document or the welcome message.
  // To do this, we must first get the cursor position, and then write it to the screen buffer before flushing it

  // We get the cursor position in the terminal by adding 1 to cursor.x and cursor.y
  // (less the corresponding offset values) to convert from 0-indexed values to the 1-indexed values
  // that the terminal uses
  auto const cursor_pos =
    fmt::format("\x1b[{};{}H", (editor.curs.y - editor.off.row) + 1, (editor.rx - editor.off.col) + 1);

  // The file to which the screen buffer writes its contents when flushed (typically STDOUT)
  io::file out_file {};

  editor.screen_buf.write(cursor_pos).write(utilities::escape_sequences::show_the_cursor).flush(out_file);
}

/**
 * \brief Draw each row of the buffer of text being edited, plus a tilde at the beginning, or the welcome message
 * \param[in] editor The editor configuration
 */
void draw_rows(editor_config& editor)
{
  for (int32_t curr_row = 0; curr_row < editor.winsize.rows; ++curr_row) {
    if (auto const file_row = curr_row + editor.off.row; file_row >= std::ssize(editor.row)) {
      if (editor.row.empty() and curr_row == editor.winsize.rows / 3) {
        detail::print_welcome_message(editor.winsize.cols, editor.screen_buf);
      }
      else {
        editor.screen_buf.write("~");
      }
    }
    else {
      detail::print_line_of_document(editor.row[static_cast<std::size_t>(file_row)].render, editor.screen_buf,
                                     editor.winsize.cols, editor.off.col);
    }

    editor.screen_buf.write(utilities::escape_sequences::erase_part_of_line_to_the_right_of_cursor);

    editor.screen_buf.write(utilities::escape_sequences::crnl);
  }
}

/**
 * \brief Move the cursor in the direction of the key pressed
 * \param[in] key The key pressed by the user
 * \param[in] editor The current state of the editor
 */
void move_cursor(utilities::editor_key const key, editor_config& editor)
{
  using enum utilities::editor_key;

  auto const get_current_row = [&editor]() noexcept -> std::string* {
    if (editor.curs.y >= std::ssize(editor.row)) {
      return nullptr;
    }

    return &editor.row[static_cast<std::size_t>(editor.curs.y)].chars;
  };

  switch (key) {
    case arrow_left:
      if (editor.curs.x != 0) {
        --editor.curs.x;
      }
      else if (editor.curs.y > 0) {
        --editor.curs.y;
        editor.curs.x = std::ssize(editor.row[static_cast<std::size_t>(editor.curs.y)].chars);
      }
      break;
    case arrow_right: {
      auto const* curr_row = get_current_row();

      if ((curr_row != nullptr) and editor.curs.x < std::ssize(*curr_row)) {
        ++editor.curs.x;
      }
      else if ((curr_row != nullptr) and editor.curs.x == std::ssize(*curr_row)) {
        ++editor.curs.y;
        editor.curs.x = 0;
      }
    } break;

    case arrow_up:
      if (editor.curs.y != 0) {
        --editor.curs.y;
      }
      break;

    case arrow_down:
      if (editor.curs.y < std::ssize(editor.row)) {
        ++editor.curs.y;
      }
      break;

    default:
      return;
  }

  auto const* curr_row = get_current_row();
  auto const row_len = (curr_row != nullptr) ? std::ssize(*curr_row) : 0;

  editor.curs.x = std::min(editor.curs.x, row_len);
}

/**
 * \brief Fix the cursor in the visible window while scrolling
 * \param[in] editor The current state of the editor
 */
void scroll(editor_config& editor) noexcept
{
  // Check if the cursor has moved outside the visible window
  // If so, adjust the editor.offset.row and/or editor.offset.col variable(s) so that the
  // cursor is just inside the visible window

  editor.rx = 0;

  if (editor.curs.y < std::ssize(editor.row)) {
    editor.rx = row_cx_to_rx(editor.row[static_cast<std::size_t>(editor.curs.y)], editor.curs.x);
  }

  editor.off.row = std::min(editor.curs.y, editor.off.row);

  if (editor.curs.y >= editor.off.row + editor.winsize.rows) {
    editor.off.row = editor.curs.y - editor.winsize.rows + 1;
  }

  editor.off.col = std::min(editor.rx, editor.off.col);

  if (editor.rx >= editor.off.col + editor.winsize.cols) {
    editor.off.col = editor.rx - editor.winsize.cols + 1;
  }
}

void open(std::filesystem::path const& path, gsl::not_null<editor_config*> editor)
{
  editor->filename = path.filename();
  auto file = std::ifstream(path);

  if (!file.is_open()) {
    throw std::ios_base::failure("Could not open file");
  }

  auto line = std::string();

  while (std::getline(file, line)) {
    if (!line.empty() and line.back() == '\r') {
      line.pop_back();
    }

    editor->row.emplace_back(line);
  }

  if ((file.bad() or file.fail()) and !file.eof()) {
    throw std::ios_base::failure("Error while reading from file");
  }

  file.close();

  for (auto& [chars, render] : editor->row) {
    update_row(chars, render);
  }
}

/**
 * \brief Copies the contents of the source string into the destination string
 * \param[in] row The source string
 * \param[in] render The destination string
 */
void update_row(std::string_view row, std::string& render) noexcept
{
  using utilities::kilo_tab_stop;

  auto tabs = std::ranges::count_if(row, [](unsigned char character) -> bool { return character == '\t'; });

  render.clear();
  render.resize(row.length() + static_cast<size_t>(tabs * (kilo_tab_stop - 1)) + 1);

  std::size_t idx {};

  for (auto const character : row) {
    if (character == '\t') {
      render[idx] = ' ';
      ++idx;

      while (idx % kilo_tab_stop != 0) {
        render[idx] = ' ';
        ++idx;
      }
    }
    else {
      render[idx] = character;
      ++idx;
    }
  }
}

auto row_cx_to_rx(erow const& row, int64_t cursor_x) -> int64_t
{
  using utilities::kilo_tab_stop;

  int64_t render_x {};

  // Loop through all the characters to the left of `cursor_x`, and figure out how many spaces each tab takes up.
  // For each character, if it's a tab we use rx % kilo_tab_stop to find out how many columns we are to the right of the
  // last tab stop, then subtract that from kilo_tab_stop - 1 to find out how many columns we are to the left of the
  // next tab stop.
  // We add that amount to render_x to get just to the left of the next tab stop, and then the unconditional ++render_x
  // statement gets us right on the next tab stop. This works even if we are currently on a tab stop.

  for (std::size_t j = 0; std::cmp_less(j, cursor_x); ++j) {
    if (row.chars[j] == '\t') {
      render_x += (kilo_tab_stop - 1) - (render_x % kilo_tab_stop);
    }

    ++render_x;
  }

  return render_x;
}

void draw_status_bar(gsl::not_null<editor_config*> editor)
{
  using utilities::escape_sequences;

  editor->screen_buf.write(escape_sequences::switch_to_inverted_colours);

  auto status =
    fmt::format("{:.20} - {} lines", editor->filename.empty() ? "[No Name]" : editor->filename, editor->row.size());
  auto rstatus = fmt::format("{}/{}", editor->curs.y + 1, editor->row.size());

  if (std::ssize(status) > editor->winsize.cols) {
    status.resize(static_cast<std::size_t>(editor->winsize.cols));
  }

  editor->screen_buf.write(status);

  for (std::size_t i = status.length(); i < static_cast<std::size_t>(editor->winsize.cols); ++i) {
    if (static_cast<std::size_t>(editor->winsize.cols) - i != rstatus.length()) {
      editor->screen_buf.write(" ");
    }
    else {
      editor->screen_buf.write(rstatus);
      break;
    }
  }

  editor->screen_buf.write(escape_sequences::switch_to_normal_formatting);
  editor->screen_buf.write(escape_sequences::crnl);
}
}

}   // namespace kilo::editor

namespace kilo::editor::detail {

/**
 * \brief Write the welcome message to the screen buffer
 *
 * \param[in] window_width The width of the window in which the message is to be displayed
 * \param[in] buffer The buffer to which the message is written before being displayed
 */
void print_welcome_message(int32_t window_width, screen_buffer& buffer)
{
  auto msg = fmt::format("Kilo editor -- version {}", utilities::kilo_version);

  // If the message is longer than the window's width, resize it to fit
  if (std::cmp_greater(msg.length(), window_width)) {
    msg.resize(static_cast<std::size_t>(window_width));
  }

  /*
   * Center the string
   * Divide the screen width by 2 and then subtract half the string's length
   * from this value. This tells us how far from the left edge of the screen we
   * should start printing the string. So, we fill that space with space
   * characters, except for the first character, which should be a tilde
   */

  auto padding = (window_width - std::ssize(msg)) / 2;

  if (padding > 0) {
    buffer.write("~");
    padding--;
  }

  while (padding > 0) {
    buffer.write(" ");
    padding--;
  }

  buffer.write(msg);
}

/**
 * \brief Print a line of text from the open document to the screen
 *
 * \param[in] line The line to be printed
 * \param[in] buffer The screen buffer
 * \param[in] window_width The width of the terminal window
 * \param[in] col_off The column offset between the terminal window width and the document width
 * \pre The column offset must be non-negative
 */
void print_line_of_document(std::string const& line, screen_buffer& buffer, int32_t window_width, int64_t col_off)
{
  Expects(col_off >= 0 and "Column offset must be non-negative");

  auto line_len = std::ssize(line) - col_off;

  line_len = std::max<int64_t>(line_len, 0);

  line_len = std::min<int64_t>(line_len, window_width);

  buffer.write({&line.c_str()[static_cast<std::size_t>(col_off)], static_cast<std::size_t>(line_len)});
}

}   // namespace kilo::editor::detail
