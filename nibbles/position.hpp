#ifndef NIBBLES__POSITION_HPP
#define NIBBLES__POSITION_HPP

#include <nibbles/utility/dataclass.hpp>

namespace nibbles {

struct Position :
  utility::DataClass<
    Position,
    uint32_t, x,
    uint32_t, y
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Position)
};

}

#endif // NIBBLES__POSITION_HPP

