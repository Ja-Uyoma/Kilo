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

#include "editor_config.hpp"

#include "kilo/editor/append_buffer/append_buffer.hpp"
#include "kilo/editor/erow/erow.hpp"
#include "kilo/io/File.hpp"
#include "kilo/terminal/window_size/window_size.hpp"
#include "kilo/utilities/Constants.hpp"
#include "kilo/utilities/Utilities.hpp"
#include <fmt/format.h>
#include <gsl/assert>
#include <gsl/pointers>
#include <string_view>

#include <algorithm>
#include <cassert>
#include <chrono>
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

namespace kilo::editor::editor_config {

void process_keypress(int key_pressed, editor_config& editor_config, terminal::window_size::window_size const& winsize)
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
      editor_config.curs.y = editor_config.off.row + winsize.rows - 1;
      editor_config.curs.y = std::min(editor_config.curs.y, std::ssize(editor_config.row));
    }

    for (int32_t i = winsize.rows; i > 0; --i) {
      move_cursor(key == page_up ? arrow_up : arrow_down, editor_config);
    }
  }
  else if (key == arrow_left or key == arrow_right or key == arrow_up or key == arrow_down) {
    move_cursor(key, editor_config);
  }
}

void refresh_screen(editor_config& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
                    terminal::window_size::window_size const& winsize)
{
  scroll(editor, winsize);

  // Hide the cursor when painting and then move it to the Home position
  abuf->write(utilities::escape_sequences::hide_cursor_when_repainting)
    .write(utilities::escape_sequences::move_cursor_to_home_position);

  // Draw the welcome message, or each row of the currently open document with a tilde at the beginning
  draw_rows(editor, abuf, winsize);
  draw_status_bar(editor, abuf, winsize);
  draw_message_bar(editor, abuf, winsize);

  // We want to show the cursor immediately after writing the contents of the open document or the welcome message.
  // To do this, we must first get the cursor position, and then write it to the screen buffer before flushing it

  // We get the cursor position in the terminal by adding 1 to cursor.x and cursor.y
  // (less the corresponding offset values) to convert from 0-indexed values to the 1-indexed values
  // that the terminal uses
  auto const cursor_pos =
    fmt::format("\x1b[{};{}H", (editor.curs.y - editor.off.row) + 1, (editor.rx - editor.off.col) + 1);

  // The file to which the screen buffer writes its contents when flushed (typically STDOUT)
  io::file out_file {};

  abuf->write(cursor_pos).write(utilities::escape_sequences::show_the_cursor).flush(out_file);
}

void draw_rows(editor_config const& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
               terminal::window_size::window_size const& winsize)
{
  for (int32_t curr_row = 0; curr_row < winsize.rows; ++curr_row) {
    if (auto const file_row = curr_row + editor.off.row; file_row >= std::ssize(editor.row)) {
      if (editor.row.empty() and curr_row == winsize.rows / 3) {
        detail::print_welcome_message(winsize.cols, abuf);
      }
      else {
        abuf->write("~");
      }
    }
    else {
      detail::print_line_of_document(editor.row[static_cast<std::size_t>(file_row)].render, abuf, winsize.cols,
                                     editor.off.col);
    }

    abuf->write(utilities::escape_sequences::erase_part_of_line_to_the_right_of_cursor);
    abuf->write(utilities::escape_sequences::crnl);
  }
}

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

void scroll(editor_config& editor, terminal::window_size::window_size const& winsize) noexcept
{
  // Check if the cursor has moved outside the visible window
  // If so, adjust the editor.offset.row and/or editor.offset.col variable(s) so that the
  // cursor is just inside the visible window

  editor.rx = 0;

  if (editor.curs.y < std::ssize(editor.row)) {
    editor.rx = row_cx_to_rx(editor.row[static_cast<std::size_t>(editor.curs.y)], editor.curs.x);
  }

  editor.off.row = std::min(editor.curs.y, editor.off.row);

  if (editor.curs.y >= editor.off.row + winsize.rows) {
    editor.off.row = editor.curs.y - winsize.rows + 1;
  }

  editor.off.col = std::min(editor.rx, editor.off.col);

  if (editor.rx >= editor.off.col + winsize.cols) {
    editor.off.col = editor.rx - winsize.cols + 1;
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

  for (auto& row : editor->row) {
    erow::update_row(row);
  }
}

void draw_status_bar(editor_config const& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
                     terminal::window_size::window_size const& winsize)
{
  using utilities::escape_sequences;

  abuf->write(escape_sequences::switch_to_inverted_colours);

  auto status =
    fmt::format("{:.20} - {} lines", editor.filename.empty() ? "[No Name]" : editor.filename, editor.row.size());
  auto rstatus = fmt::format("{}/{}", editor.curs.y + 1, editor.row.size());

  if (std::ssize(status) > winsize.cols) {
    status.resize(static_cast<std::size_t>(winsize.cols));
  }

  abuf->write(status);

  for (std::size_t i = status.length(); i < static_cast<std::size_t>(winsize.cols); ++i) {
    if (static_cast<std::size_t>(winsize.cols) - i != rstatus.length()) {
      abuf->write(" ");
    }
    else {
      abuf->write(rstatus);
      break;
    }
  }

  abuf->write(escape_sequences::switch_to_normal_formatting);
  abuf->write(escape_sequences::crnl);
}

void draw_message_bar(editor_config const& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
                      terminal::window_size::window_size const& winsize)
{
  using std::chrono::system_clock;
  using utilities::escape_sequences;

  static constexpr auto time_limit = 5U;

  abuf->write(escape_sequences::erase_part_of_line_to_the_right_of_cursor);
  auto const msg_len = std::min(std::ssize(editor.status_msg), static_cast<int64_t>(winsize.cols));

  if (msg_len > 0 and system_clock::now() - editor.status_msg_time < std::chrono::seconds(time_limit)) {
    abuf->write({editor.status_msg.c_str(), static_cast<std::size_t>(msg_len)});
  }
}

}   // namespace kilo::editor::editor_config

namespace kilo::editor::editor_config::detail {

void print_welcome_message(int32_t window_width, gsl::not_null<append_buffer::append_buffer*> buffer)
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
    buffer->write("~");
    padding--;
  }

  while (padding > 0) {
    buffer->write(" ");
    padding--;
  }

  buffer->write(msg);
}

void print_line_of_document(std::string const& line, gsl::not_null<append_buffer::append_buffer*> buffer,
                            int32_t window_width, int64_t col_off)
{
  Expects(col_off >= 0 and "Column offset must be non-negative");

  auto line_len = std::ssize(line) - col_off;

  line_len = std::max<int64_t>(line_len, 0);

  line_len = std::min<int64_t>(line_len, window_width);

  buffer->write({&line.c_str()[static_cast<std::size_t>(col_off)], static_cast<std::size_t>(line_len)});
}

}   // namespace kilo::editor::editor_config::detail
