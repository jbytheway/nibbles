#include <nibbles/snake.hpp>

#include <nibbles/board.hpp>

namespace nibbles {

Point Snake::nextHead(Board const& board) const
{
  Point front = get<points>().front();
  return board.adjacent(front, get<direction>());
}

TickResult Snake::advanceHead(Board& board, std::vector<Point> const& nextHeads)
{
  Point next = nextHead(board);
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
  auto& pts = get<points>();
  assert(!pts.empty());
  board[pts.back()] = BoardState::empty;
  pts.pop_back();
}

}

