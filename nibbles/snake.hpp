#ifndef NIBBLES__SNAKE_HPP
#define NIBBLES__SNAKE_HPP

#include <nibbles/position.hpp>
#include <nibbles/playerid.hpp>

namespace nibbles {

struct Snake :
  utility::DataClass<
    Snake,
    PlayerId, player,
    std::vector<Position>, positions
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Snake)
};

}

#endif // NIBBLES__SNAKE_HPP

