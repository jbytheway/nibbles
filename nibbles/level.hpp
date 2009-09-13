#ifndef NIBBLES__LEVEL_HPP
#define NIBBLES__LEVEL_HPP

#include <random>

#include <boost/foreach.hpp>

#include <nibbles/leveldefinition.hpp>
#include <nibbles/snake.hpp>
#include <nibbles/number.hpp>
#include <nibbles/board.hpp>
#include <nibbles/tickresult.hpp>

namespace nibbles {

struct Level :
  utility::DataClass<
    Level,
    LevelDefinition, definition,
    std::vector<Snake>, snakes,
    Number, number,
    Board, board
  > {
  Level() {}
  //NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Level)

  template<typename Container, typename RandomEngine>
  Level(
      const LevelDefinition&,
      const Container& playerIds,
      RandomEngine&
    );

    template<typename RandomEngine>
    TickResult tick(RandomEngine&);
  private:
    template<typename RandomEngine>
    void randomNumber(RandomEngine& random, uint32_t value);
    void initBoard() { get<board>().init(get<definition>()); }
};

template<typename Container, typename RandomEngine>
Level::Level(
    const LevelDefinition& def,
    const Container& playerIds,
    RandomEngine& random
  ) :
  base(def, std::vector<Snake>(), Number(), Board())
{
  const std::vector<Position>& starts =
    get<definition>().get<fields::starts>();
  if (starts.size() < playerIds.size()) {
    throw std::logic_error("level has insufficient starts");
  }
  size_t start = 0;
  BOOST_FOREACH(const PlayerId playerId, playerIds) {
    get<snakes>().push_back(Snake(playerId, starts[start]));
    ++start;
  }

  initBoard();
  randomNumber(random, 1);
}

template<typename RandomEngine>
void Level::randomNumber(RandomEngine& random, uint32_t value)
{
  const LevelDefinition& def = get<definition>();
  const Board& board = get<fields::board>();
  // These are *inclusive* maxima
  size_t maxX = def.get<w>()-Number::width;
  size_t maxY = def.get<h>()-Number::height;

  // FIXME: These names might change to uniform_int_distribution in final std
  std::uniform_int<size_t> xDist(0, maxX);
  std::uniform_int<size_t> yDist(0, maxY);

  Block trialBlock;

  bool free;
  do {
    trialBlock =
      Block(xDist(random), yDist(random), Number::width, Number::height);
    free = true;
    for (Block::iterator i = trialBlock.begin(); i != trialBlock.end(); ++i) {
      if (board[*i] != BoardState::empty) {
        free = false;
        break;
      }
    }
  } while (!free);
  get<number>() = Number(trialBlock, value);
}

template<typename RandomEngine>
TickResult Level::tick(RandomEngine& random)
{
  TickResult overallResult = TickResult::none;
  std::vector<Snake>& snakes = get<fields::snakes>();
  Board& b = get<board>();

  std::vector<Point> nextHeads;
  BOOST_FOREACH(Snake& snake, snakes) {
    nextHeads.push_back(snake.nextHead());
  }
  BOOST_FOREACH(Snake& snake, snakes) {
    TickResult result = snake.advanceHead(b, nextHeads);
    overallResult = std::max(overallResult, result);
  }

  BOOST_FOREACH(Snake& snake, snakes) {
    snake.advanceTail(b);
  }

  if (overallResult == TickResult::number) {
    randomNumber(random, get<number>().get<value>());
  }

  return overallResult;
}

}

#endif // NIBBLES__LEVEL_HPP

