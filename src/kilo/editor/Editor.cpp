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
#include <string_view>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
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
    editor_config.curs.x = editor_config.winsize.cols - 1;
  }
  else if (key == page_up or key == page_down) {
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
  // Hide the cursor when painting and then move it to the Home position
  editor.screen_buf.write(utilities::escape_sequences::hide_cursor_when_repainting)
    .write(utilities::escape_sequences::move_cursor_to_home_position);

  // Draw the welcome message, or each row of the currently open document with a tilde at the beginning
  draw_rows(editor);

  // We want to show the cursor immediately after writing the contents of the open document or the welcome message.
  // To do this, we must first get the cursor position, and then write it to the screen buffer before flushing it

  // We get the cursor position in the terminal by adding 1 to cursor.x and cursor.y
  // (less the corresponding offset values) to convert from 0-indexed values to the 1-indexed values
  // that the terminal uses
  auto const cursor_pos =
    fmt::format("\x1b[{};{}H", (editor.curs.y - editor.off.row) + 1, (editor.curs.x - editor.off.col) + 1);

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
    if (auto const file_row = curr_row + editor.off.row; file_row >= std::ssize(editor.open_doc)) {
      if (editor.open_doc.empty() and curr_row == editor.winsize.rows / 3) {
        detail::print_welcome_message(editor.winsize.cols, editor.screen_buf);
      }
      else {
        editor.screen_buf.write("~");
      }
    }
    else {
      detail::print_line_of_document(editor.render[static_cast<std::size_t>(file_row)], editor.screen_buf,
                                     editor.winsize.cols, editor.off.col);
    }

    editor.screen_buf.write(utilities::escape_sequences::erase_part_of_line_to_the_right_of_cursor);

    if (curr_row < editor.winsize.rows - 1) {
      editor.screen_buf.write(utilities::escape_sequences::crnl);
    }
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

  switch (key) {
    case arrow_left:
      if (editor.curs.x != 0) {
        --editor.curs.x;
      }
      else if (editor.curs.y > 0) {
        --editor.curs.y;
        editor.curs.x = std::ssize(editor.open_doc[static_cast<std::size_t>(editor.curs.y)]);
      }
      break;
    case arrow_right: {
      auto const curr_row = std::invoke([&editor]() -> std::optional<std::string> {
        if (editor.curs.y >= std::ssize(editor.open_doc)) {
          return std::nullopt;
        }
        return std::make_optional(editor.open_doc[static_cast<std::size_t>(editor.curs.y)]);
      });

      if (curr_row and editor.curs.x < std::ssize(*curr_row)) {
        ++editor.curs.x;
      }
      else if (curr_row and editor.curs.x == std::ssize(*curr_row)) {
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
      if (editor.curs.y < std::ssize(editor.open_doc)) {
        ++editor.curs.y;
      }
      break;

    default:
      return;
  }

  auto const curr_row = std::invoke([&editor]() -> std::optional<std::string> {
    if (editor.curs.y >= std::ssize(editor.open_doc)) {
      return std::nullopt;
    }
    return std::make_optional(editor.open_doc[static_cast<std::size_t>(editor.curs.y)]);
  });

  auto const row_len = curr_row ? std::ssize(*curr_row) : 0;

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

  editor.off.row = std::min(editor.curs.y, editor.off.row);

  if (editor.curs.y >= editor.off.row + editor.winsize.rows) {
    editor.off.row = editor.curs.y - editor.winsize.rows + 1;
  }

  editor.off.col = std::min(editor.curs.x, editor.off.col);

  if (editor.curs.x >= editor.off.col + editor.winsize.cols) {
    editor.off.col = editor.curs.x - editor.winsize.cols + 1;
  }
}

/**
 * \brief Open a file and write its contents to memory
 *
 * \param[in] path The path to the file
 * \param[in] document The buffer containing the file in memory
 * \param[in] rendered The document that is actually rendered to the window
 * \return true If the operation was successful, and false otherwise
 */
auto open(std::filesystem::path const& path, std::vector<std::string>& document, std::vector<std::string>& rendered)
  -> bool
{
  if (!std::filesystem::is_regular_file(path)) {
    return false;
  }

  std::ifstream infile(path);

  if (!infile) {
    return false;
  }

  std::string line;

  while (std::getline(infile, line)) {
    document.push_back(line);
  }

  rendered = document;

  return true;
}

/**
 * \brief Copies the contents of the source string into the destination string
 * \param[in] row The source string
 * \param[in] render The destination string
 */
void update_row(std::string_view row, std::string& render) noexcept
{
  Expects(row.length() == render.length() and "The source and destination strings must be of the same size");

  using utilities::kilo_tab_stop;

  [[maybe_unused]] auto tabs =
    std::ranges::count_if(row, [](unsigned char character) -> bool { return character == '\t'; });

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

  auto line_len = std::clamp<int64_t>(std::ssize(line) - col_off, 0, static_cast<int64_t>(window_width));

  buffer.write(line.substr(static_cast<size_t>(col_off), static_cast<size_t>(line_len)));
}

}   // namespace kilo::editor::detail
