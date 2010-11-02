#ifndef NIBBLES__BOARD_HPP
#define NIBBLES__BOARD_HPP

#include <boost/multi_array.hpp>

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>
#include <nibbles/point.hpp>
#include <nibbles/boardstate.hpp>
#include <nibbles/leveldefinition.hpp>

namespace nibbles {

struct Board :
  utility::DataClass<
    Board,
    boost::multi_array<BoardState, 2>, states
  > {
  Board() {}
  Board& operator=(const Board&);
  const BoardState& operator[](const Point& p) const {
    return get<states>()[p.get<x>()][p.get<y>()];
  }
  BoardState& operator[](const Point& p) {
    return get<states>()[p.get<x>()][p.get<y>()];
  }

  void init(const LevelDefinition&);
  void assign(const Block&, const BoardState);
  Point adjacent(Point const&, Direction const) const;

  // for debugging - print board to cout
  void dumpBoard();
};

}

#endif // NIBBLES__BOARD_HPP

