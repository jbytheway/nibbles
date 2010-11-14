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
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Color)

  double d_red() const { return get<fields::red>()/255.0; }
  double d_green() const { return get<fields::green>()/255.0; }
  double d_blue() const { return get<fields::blue>()/255.0; }

  static Color black;
  static Color blue;
  static Color green;
  static Color cyan;
  static Color red;
  static Color magenta;
  static Color yellow;
  static Color white;
};

}

#endif // NIBBLES__COLOR_HPP

