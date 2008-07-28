#ifndef NIBBLES__POINT_HPP
#define NIBBLES__POINT_HPP

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>

namespace nibbles {

struct Point :
  utility::DataClass<
    Point,
    uint32_t, x,
    uint32_t, y
  > {
  typedef uint32_t value_type;
  
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Point)

  Point& operator-=(const Point& p) {
    get<x>() -= p.get<x>();
    get<y>() -= p.get<y>();
    return *this;
  }

  bool operator==(const Point& right) const {
    return get<x>() == right.get<x>() && get<y>() == right.get<y>();
  }
};

}

#endif // NIBBLES__POINT_HPP

