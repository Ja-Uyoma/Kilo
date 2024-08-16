#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <string_view>

namespace Kilo {

class Text
{
public:
  std::string original {};
  std::string rendered {};

  /**
   * @brief Create a default Text instance
   *
   */
  explicit constexpr Text() noexcept = default;

  /**
   * @brief Create a Text instance from the given string
   * @param[in] str The string from which the Text object is to be constructed
   *
   */
  explicit constexpr Text(std::string_view str) noexcept : original(str)
  {
  }
};

}   // namespace Kilo

#endif
