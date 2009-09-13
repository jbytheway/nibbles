#include <nibbles/snake.hpp>

#include <nibbles/board.hpp>

namespace nibbles {

Point Snake::nextHead() const
{
  Point front = get<points>().front();
  return front.moved(get<direction>());
}

TickResult Snake::advanceHead(Board& board, std::vector<Point> const& nextHeads)
{
  Point next = nextHead();
  size_t numNextHeads = std::count(nextHeads.begin(), nextHeads.end(), next);
  if (numNextHeads > 1) {
    return TickResult::dead;
  }

  switch (board[next]) {
    case BoardState::wall:
    case BoardState::snake:
      return TickResult::dead;
    case BoardState::empty:
      get<points>().push_front(next);
      board[next] = BoardState::snake;
      return TickResult::none;
    default:
      throw std::logic_error("unexpected BoardState");
  }
}

void Snake::advanceTail(Board& board)
{
  board[get<points>().back()] = BoardState::empty;
  get<points>().pop_back();
}

}

