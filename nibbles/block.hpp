#ifndef NIBBLES__BLOCK_HPP
#define NIBBLES__BLOCK_HPP

#include <nibbles/point.hpp>

namespace nibbles {

struct Block :
  utility::DataClass<
    Block,
    Point, fields::min,
    Point, fields::max
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Block)

  template<typename Int>
  Block(Int x, Int y, Int w, Int h) :
    base(Point(x, y), Point(x+w, y+h)) {}

  Block& operator-=(const Point& p) {
    get<min>() -= p;
    get<max>() -= p;
    return *this;
  }
};

}

#endif // NIBBLES__BLOCK_HPP

