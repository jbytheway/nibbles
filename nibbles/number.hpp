#ifndef NIBBLES__NUMBER_HPP
#define NIBBLES__NUMBER_HPP

#include <nibbles/position.hpp>

namespace nibbles {

struct Number :
  utility::DataClass<
    Number,
    Block, position,
    uint32_t, value
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Number)

  static const size_t width = 1;
  static const size_t height = 1;
};

}

#endif // NIBBLES__NUMBER_HPP

