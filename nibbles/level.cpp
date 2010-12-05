#include <nibbles/level.hpp>

namespace nibbles {

TickResult Level::tick(
  GameSettings const& settings,
  ScoreHandler& scorer,
  Moves const& moves
)
{
  assert(std::is_sorted(moves.begin(), moves.end()));
  TickResult overallResult = TickResult::none;
  std::vector<Snake>& snakes = get<fields::snakes>();
  Board& b = get<board>();

  std::vector<Point> nextHeads;
  BOOST_FOREACH(Snake& snake, snakes) {
    using boost::phoenix::arg_names::arg1;
    auto it = std::find_if(
      moves.begin(), moves.end(),
      (&arg1)->*&Moves::value_type::first == snake.get<player>()
    );
    if (it != moves.end()) {
      snake.turn(it->second);
    }
    nextHeads.push_back(snake.nextHead(b));
  }
  BOOST_FOREACH(Snake& snake, snakes) {
    TickResult const result = snake.advanceHead(b, nextHeads);
    if (result == TickResult::number) {
      auto value = get<number>().get<fields::value>();
      scorer(snake.get<player>(), value);
      snake.get<pendingGrowth>() += value*settings.get<growthFactor>();
    }
    overallResult = std::max(overallResult, result);
  }

  BOOST_FOREACH(Snake& snake, snakes) {
    snake.advanceTail(b);
  }

  return overallResult;
}

void Level::setNumber(Number const& newNumber)
{
  Board& board = get<fields::board>();
  Number& oldNumber = get<fields::number>();

  // Clear the old number
  BOOST_FOREACH(auto const& point, oldNumber.get<position>()) {
    BoardState& state = board[point];
    if (state == BoardState::number) state = BoardState::empty;
  }

  oldNumber = newNumber;

  // Set board states
  BOOST_FOREACH(auto const& point, oldNumber.get<position>()) {
    board[point] = BoardState::number;
  }
}

}

