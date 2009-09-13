#ifndef NIBBLES__SNAKE_HPP
#define NIBBLES__SNAKE_HPP

#include <deque>

#include <nibbles/direction.hpp>
#include <nibbles/position.hpp>
#include <nibbles/point.hpp>
#include <nibbles/playerid.hpp>
#include <nibbles/tickresult.hpp>

namespace nibbles {

struct Board;

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

  Point nextHead() const;
  TickResult advanceHead(Board&, std::vector<Point> const& nextHeads);
  void advanceTail(Board&);
};

}

#endif // NIBBLES__SNAKE_HPP

