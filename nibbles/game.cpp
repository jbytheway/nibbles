#include <nibbles/game.hpp>

using namespace std;

namespace nibbles {

void Game::startLevel(LevelId levelId) {
  const vector<LevelDefinition>& levelDefs = get<levels>().get<levels>();
  assert(!levelDefs.empty());
  LevelId realLevelId =
    LevelId::fromInteger(std::min<size_t>(levelId, levelDefs.size()-1));
  const LevelDefinition& levelDef = levelDefs[realLevelId];
  get<level>() = Level(levelDef, get<players>(), get<random>());
}

}

