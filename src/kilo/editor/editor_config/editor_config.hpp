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

#ifndef KILO_EDITOR_EDITOR_CONFIG_EDITOR_CONFIG_HPP
#define KILO_EDITOR_EDITOR_CONFIG_EDITOR_CONFIG_HPP

#include "kilo/editor/append_buffer/append_buffer.hpp"
#include "kilo/editor/cursor/cursor.hpp"
#include "kilo/editor/erow/erow.hpp"
#include "kilo/editor/offset/offset.hpp"
#include "kilo/terminal/window_size/window_size.hpp"
#include "kilo/utilities/Constants.hpp"
#include <fmt/format.h>
#include <gsl/pointers>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace kilo::editor::editor_config {

/**
 * @struct editor_config
 * @brief Represents the state of the editor
 */
struct editor_config
{
  cursor::cursor curs;
  // Index into the `render` string
  int64_t rx {};
  offset::offset off;
  std::vector<erow::erow> row;
  std::string filename;
  std::string status_msg;
  std::chrono::time_point<std::chrono::system_clock> status_msg_time;
};

/**
 * @brief Move the cursor in the open document depending on the key pressed
 * @param[in] key_pressed The key pressed by the user
 * @param[in] editor The current state of the editor
 */
void process_keypress(int key_pressed, editor_config& editor_config, terminal::window_size::window_size const& winsize);

/**
 * @brief Perform a screen refresh
 * @param[in] editor The current editor configuration
 * @param[in,out] abuf The buffer to which writes to the screen are done
 */
void refresh_screen(editor_config& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
                    terminal::window_size::window_size const& winsize);

/**
 * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning, or the welcome message
 * @param[in] editor The editor configuration
 * @param[in,out] abuf The buffer to which writes to the screen are done
 */
void draw_rows(editor_config const& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
               terminal::window_size::window_size const& winsize);

/**
 * @brief Move the cursor in the direction of the key pressed
 * @param[in] key The key pressed by the user
 * @param[in] editor The current state of the editor
 */
void move_cursor(utilities::editor_key key, editor_config& editor);

/**
 * @brief Fix the cursor in the visible window while scrolling
 * @param[in] editor The current state of the editor
 */
void scroll(editor_config& editor, terminal::window_size::window_size const& winsize) noexcept;

/**
 * @brief Open a file and write its contents to a buffer in memory
 *
 * @param[in] path The path to the file
 * @param[in,out] editor The current editor state
 *
 * @throws std::ios_base::failure if an error was encountered when opening or reading from the file
 */
void open(std::filesystem::path const& path, gsl::not_null<editor_config*> editor);

/**
 * @brief Draw a status bar with inverted colours at the bottom of the screen
 * @param[in] editor The current editor state
 * @param[in,out] abuf The buffer to which writes to the screen are done
 */
void draw_status_bar(editor_config const& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
                     terminal::window_size::window_size const& winsize);

/**
 * @brief Set a status message to be displayed in the editor
 * @tparam Args The type of arguments to pass to the format string
 * @param[in] editor The current state of the editor
 * @param[in] args A variable list of arguments to be appended to the status message
 */
template<typename... Args>
void set_status_msg(gsl::not_null<editor_config*> editor, fmt::format_string<Args...> fmt_str, Args&&... args)
{
  editor->status_msg = fmt::format(fmt_str, std::forward<Args>(args)...);
  editor->status_msg_time = std::chrono::system_clock::now();
}

/**
 * @brief Draw the message bar
 * @param[in] editor The current state of the editor
 * @param[in,out] abuf The buffer to which writes to the screen are done
 */
void draw_message_bar(editor_config const& editor, gsl::not_null<append_buffer::append_buffer*> abuf,
                      terminal::window_size::window_size const& winsize);

/**
 * @brief Insert a single character into a row of the editor
 * @param[in] editor The editor's state
 * @param[in] character The character to be inserted
 */
void insert_char(editor_config& editor, int character);

}   // namespace kilo::editor::editor_config

namespace kilo::editor::editor_config::detail {

/**
 * @brief Write the welcome message to the screen buffer
 * @param[in] window_width The width of the window in which the message is to be displayed
 * @param[in,out] buffer The buffer to which the message is written before being displayed
 */
void print_welcome_message(int32_t window_width, gsl::not_null<append_buffer::append_buffer*> buffer);

/**
 * @brief Print a line of text from the open document to the screen
 * @param[in] line The line to be printed
 * @param[in,out] buffer The buffer to which writes to the screen are done
 * @param[in] window_width The width of the terminal window
 * @param[in] col_off The column offset between the terminal window width and the document width
 * @pre The column offset must be non-negative
 */
void print_line_of_document(std::string const& line, gsl::not_null<append_buffer::append_buffer*> buffer,
                            int32_t window_width, int64_t col_off);

}   // namespace kilo::editor::editor_config::detail

#endif
