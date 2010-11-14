#ifndef NIBBLES__DIRECTION_HPP
#define NIBBLES__DIRECTION_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles {

#define NIBBLES_DIRECTION_VALUES() (up)(down)(left)(right)
NIBBLES_UTILITY_ENUM_HEADER(Direction, NIBBLES_DIRECTION_VALUES())

inline Direction directionOpposite(Direction d) {
  return Direction(d^1);
}

}

#endif // NIBBLES__DIRECTION_HPP

