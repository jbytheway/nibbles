#ifndef NIBBLES__LEVEL_HPP
#define NIBBLES__LEVEL_HPP

#include <random>

#include <boost/foreach.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <boost/spirit/home/phoenix/operator/member.hpp>
#include <boost/spirit/home/phoenix/operator/comparison.hpp>

#include <nibbles/leveldefinition.hpp>
#include <nibbles/snake.hpp>
#include <nibbles/number.hpp>
#include <nibbles/board.hpp>
#include <nibbles/tickresult.hpp>
#include <nibbles/scorehandler.hpp>
#include <nibbles/gameeventhandler.hpp>
#include <nibbles/gamesettings.hpp>

namespace nibbles {

class Level :
  public utility::DataClass<
    Level,
    LevelDefinition, definition,
    std::vector<Snake>, snakes,
    Number, number,
    Board, board
  > {
  public:
    Level() {}

    template<typename Range, typename RandomEngine>
    Level(
      GameSettings const&,
      const LevelDefinition&,
      const Range& playerIds,
      RandomEngine&,
      GameEventHandler&
    );

    template<typename Range>
    Level(
      GameSettings const&,
      const LevelDefinition&,
      const Range& playerIds
    );

    template<typename RandomEngine>
    TickResult tick(
      GameSettings const&,
      RandomEngine&,
      ScoreHandler&,
      GameEventHandler&,
      Moves const&
    );

    TickResult tick(GameSettings const&, ScoreHandler&, Moves const&);

    void setNumber(Number const&);
  private:
    template<typename RandomEngine>
    void randomNumber(RandomEngine& random, uint32_t value, GameEventHandler&);

    template<typename Range>
    void initSnakes(GameSettings const&, Range const& playerIds);

    void initBoard() { get<board>().init(get<definition>()); }
};

template<typename Range, typename RandomEngine>
Level::Level(
    GameSettings const& settings,
    const LevelDefinition& def,
    const Range& playerIds,
    RandomEngine& random,
    GameEventHandler& handler
  ) :
  base(def, std::vector<Snake>(), Number(), Board())
{
  initSnakes(settings, playerIds);
  initBoard();
  randomNumber(random, 1, handler);
}

template<typename Range>
Level::Level(
    GameSettings const& settings,
    const LevelDefinition& def,
    const Range& playerIds
  ) :
  base(def, std::vector<Snake>(), Number(), Board())
{
  initSnakes(settings, playerIds);
  initBoard();
}

template<typename RandomEngine>
void Level::randomNumber(
  RandomEngine& random,
  uint32_t value,
  GameEventHandler& handler
)
{
  const LevelDefinition& def = get<definition>();
  Board& board = get<fields::board>();
  Number& number = get<fields::number>();

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
  setNumber(Number(trialBlock, value));
  handler.newNumber(number);
}

template<typename RandomEngine>
TickResult Level::tick(
  GameSettings const& settings,
  RandomEngine& random,
  ScoreHandler& scorer,
  GameEventHandler& handler,
  Moves const& moves
)
{
  TickResult result = tick(settings, scorer, moves);

  if (result == TickResult::number) {
    auto const oldValue = get<number>().get<value>();
    if (oldValue == 9) {
      result = TickResult::advanceLevel;
    } else {
      randomNumber(random, oldValue + 1, handler);
    }
  }

  return result;
}

template<typename Range>
void Level::initSnakes(GameSettings const& settings, Range const& playerIds)
{
  const std::vector<Position>& starts =
    get<definition>().get<fields::starts>();
  if (starts.size() < playerIds.size()) {
    throw std::logic_error("level has insufficient starts");
  }
  // TODO: make this value customizable
  SnakeLength startLength = settings.get<fields::startLength>();
  size_t start = 0;
  BOOST_FOREACH(const PlayerId playerId, playerIds) {
    get<snakes>().push_back(Snake(playerId, starts[start], startLength));
    ++start;
  }
}

}

#endif // NIBBLES__LEVEL_HPP

