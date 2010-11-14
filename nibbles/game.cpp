#include <nibbles/game.hpp>

using namespace std;

namespace nibbles {

void Game::startLevel(LevelId levelId, GameEventHandler& handler)
{
  const vector<LevelDefinition>& levelDefs = get<levels>().get<levels>();
  assert(!levelDefs.empty());
  LevelId realLevelId =
    LevelId::fromInteger(std::min<size_t>(levelId, levelDefs.size()-1));
  const LevelDefinition& levelDef = levelDefs[realLevelId];
  handler.startLevel(levelDef);
  get<level>() = Level(levelDef, get<players>(), get<random>(), handler);
  get<fields::levelId>() = levelId;
}

void Game::tick(GameEventHandler& handler)
{
  auto tickResult = get<level>().tick(get<random>(), handler);
  if (tickResult == TickResult::dead) {
    startLevel(get<levelId>(), handler);
  }
}

}

