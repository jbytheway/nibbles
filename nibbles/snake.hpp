#ifndef NIBBLES__SNAKE_HPP
#define NIBBLES__SNAKE_HPP

#include <nibbles/point.hpp>
#include <nibbles/playerid.hpp>

namespace nibbles {

struct Snake :
  utility::DataClass<
    Snake,
    PlayerId, player,
    std::vector<Point>, points
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Snake)
};

}

#endif // NIBBLES__SNAKE_HPP

