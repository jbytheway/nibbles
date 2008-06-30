#ifndef NIBBLES__COLOR_HPP
#define NIBBLES__COLOR_HPP

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>

namespace nibbles {

struct Color :
  utility::DataClass<
      Color,
      uint8_t, red,
      uint8_t, green,
      uint8_t, blue
    >
{
  template<typename... Args>
  explicit Color(Args&&... args) : base(args...) {}

  static Color black;
  static Color yellow;
};

}

#endif // NIBBLES__COLOR_HPP

