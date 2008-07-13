#ifndef NIBBLES__NUMBER_HPP
#define NIBBLES__NUMBER_HPP

#include <nibbles/position.hpp>

namespace nibbles {

struct Number :
  utility::DataClass<
    Number,
    uint32_t, value,
    std::vector<Position>, positions
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Number)
};

}

#endif // NIBBLES__NUMBER_HPP

