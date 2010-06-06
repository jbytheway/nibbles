#include <nibbles/board.hpp>

#include <iostream>

#include <boost/foreach.hpp>

using namespace std;

namespace nibbles {

Board& Board::operator=(const Board& copy)
{
  boost::multi_array<BoardState, 2>& array = get<states>();
  const boost::multi_array<BoardState, 2>& copyArray = copy.get<states>();
  array.resize(boost::extents[copyArray.shape()[0]][copyArray.shape()[1]]);
  array = copyArray;
  return *this;
}

void Board::init(const LevelDefinition& definition)
{
  const uint32_t w = definition.get<fields::w>();
  const uint32_t h = definition.get<fields::h>();
  assert(w > 0);
  assert(h > 0);
  boost::multi_array<BoardState, 2>& array = get<states>();
  array.resize(boost::extents[w][h]);

  Block whole(0, 0, w, h);
  assign(whole, BoardState::empty);
  Block top(0, 0, w, 1);
  Block bottom(0, h-1, w, 1);
  Block left(0, 0, 1, h);
  Block right(w-1, 0, 1, h);
  assign(top, BoardState::wall);
  assign(bottom, BoardState::wall);
  assign(left, BoardState::wall);
  assign(right, BoardState::wall);
  BOOST_FOREACH(const Block& wall, definition.get<blocks>()) {
    assign(wall, BoardState::wall);
  }
  dumpBoard();
}

void Board::assign(const Block& block, const BoardState state)
{
  BOOST_FOREACH(const Point& p, block) {
    (*this)[p] = state;
  }
}

void Board::dumpBoard()
{
  boost::multi_array<BoardState, 2>& array = get<states>();
  const uint32_t w = array.shape()[0];
  const uint32_t h = array.shape()[1];

  for (uint32_t x = 0; x<w; ++x) {
    for (uint32_t y = 0; y<h; ++y) {
      switch(array[x][y]) {
        case BoardState::empty:
          cout << '.';
          break;
        case BoardState::wall:
          cout << '#';
          break;
        default:
          cout << '!';
          break;
      }
    }
    cout << endl;
  }
}

}
