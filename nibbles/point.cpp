#include <nibbles/point.hpp>

namespace nibbles {

Point Point::moved(Direction const dir) const {
  switch (dir) {
    case Direction::up:
      return Point(get<x>(), get<y>()+1);
    case Direction::down:
      return Point(get<x>(), get<y>()-1);
    case Direction::left:
      return Point(get<x>()+1, get<y>());
    case Direction::right:
      return Point(get<x>()-1, get<y>());
    default:
      NIBBLES_FATAL("unexpected Direction");
  }
}

}

