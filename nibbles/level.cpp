#include <nibbles/level.hpp>

namespace nibbles {

TickResult Level::tick(Moves const& moves)
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
    TickResult result = snake.advanceHead(b, nextHeads);
    overallResult = std::max(overallResult, result);
  }

  BOOST_FOREACH(Snake& snake, snakes) {
    snake.advanceTail(b);
  }

  return overallResult;
}

}

