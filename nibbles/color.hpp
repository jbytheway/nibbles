#ifndef NIBBLES__COLOR_HPP
#define NIBBLES__COLOR_HPP

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>

namespace nibbles {

class Color :
  utility::DataClass<
      uint8_t, red,
      uint8_t, green,
      uint8_t, blue
    >
{
};

}

#endif // NIBBLES__COLOR_HPP

