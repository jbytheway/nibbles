#ifndef NIBBLES__POINT_HPP
#define NIBBLES__POINT_HPP

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>
#include <nibbles/direction.hpp>

namespace nibbles {

struct Point :
  utility::DataClass<
    Point,
    uint32_t, x,
    uint32_t, y
  > {
  typedef uint32_t value_type;

  Point() : base(-1, -1) {}

  Point(uint32_t const x, uint32_t const y) : base(x, y) {}

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

