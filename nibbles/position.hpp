#ifndef NIBBLES__POSITION_HPP
#define NIBBLES__POSITION_HPP

#include <nibbles/point.hpp>
#include <nibbles/direction.hpp>

namespace nibbles {

struct Position :
  utility::DataClass<
    Position,
    Point, point,
    Direction, direction
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Position)

  Position& operator-=(const Point& p) {
    get<point>() -= p;
    return *this;
  }
};

}

#endif // NIBBLES__POSITION_HPP

