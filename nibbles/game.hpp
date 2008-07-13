#ifndef NIBBLES__GAME_HPP
#define NIBBLES__GAME_HPP

#include <nibbles/levelpack.hpp>
#include <nibbles/level.hpp>

namespace nibbles {

struct Game :
  utility::DataClass<
    Game,
    LevelPack, levels,
    LevelId, levelId,
    Level, level
  > {
  Game() : base(LevelPack(), LevelId::invalid(), Level()) {}
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Game)

  bool started() const { return get<levelId>().valid(); }
};

}

#endif // NIBBLES__GAME_HPP

