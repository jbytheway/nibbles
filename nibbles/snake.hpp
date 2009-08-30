#ifndef NIBBLES__SNAKE_HPP
#define NIBBLES__SNAKE_HPP

#include <deque>

#include <nibbles/point.hpp>
#include <nibbles/playerid.hpp>
#include <nibbles/tickresult.hpp>

namespace nibbles {

struct Snake :
  utility::DataClass<
    Snake,
    PlayerId, player,
    std::deque<Point>, points,
    Direction, direction
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Snake)

  Snake(PlayerId id, const Position& pos) :
    base(id, std::deque<Point>(1, pos.get<point>()), pos.get<direction>())
  {}

  TickResult advanceHead();
  void advanceTail();
};

}

#endif // NIBBLES__SNAKE_HPP

