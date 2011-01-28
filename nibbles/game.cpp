#include <nibbles/game.hpp>

namespace nibbles {

void Game::startLevel(LevelId levelId, GameEventHandler& handler)
{
  const auto& levelDefs = get<levels>().get<levels>();
  assert(!levelDefs.empty());
  LevelId realLevelId =
    LevelId::fromInteger(std::min<size_t>(levelId, levelDefs.size()-1));
  const LevelDefinition& levelDef = levelDefs[realLevelId].realise();
  handler.startLevel(levelDef);
  get<level>() =
    Level(get<settings>(), levelDef, get<players>(), get<random>(), handler);
  get<fields::levelId>() = levelId;
}

TickResult Game::tick(
  ScoreHandler& scorer,
  GameEventHandler& handler,
  Moves const& moves
)
{
  auto tickResult =
    get<level>().tick(get<settings>(), get<random>(), scorer, handler, moves);
  if (tickResult >= TickResult::number) {
    // Can't use *= operator directly becasue that converts to an int and
    // rounds to zero...
    uint64_t us = get<tickInterval>().total_microseconds();
    us *= get<settings>().get<tickIntervalFactor>();
    get<tickInterval>() = boost::posix_time::microseconds(us);
  }
  switch (tickResult) {
    case TickResult::dead:
      get<tickInterval>() = get<settings>().get<tickInterval>();
      startLevel(get<levelId>(), handler);
      break;
    case TickResult::advanceLevel:
      {
        auto level = get<levelId>();
        ++level;
        startLevel(level, handler);
      }
      break;
    default:
      break;
  }
  return tickResult;
}

}

