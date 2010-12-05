#ifndef NIBBLES__GAME_HPP
#define NIBBLES__GAME_HPP

#include <boost/foreach.hpp>

#include <nibbles/levelpack.hpp>
#include <nibbles/level.hpp>
#include <nibbles/gamesettings.hpp>
#include <nibbles/idedplayer.hpp>
#include <nibbles/gameeventhandler.hpp>

namespace nibbles {

typedef std::mt19937 RandomEngine;

struct Game :
  utility::DataClass<
    Game,
    LevelPack, levels,
    GameSettings, settings,
    RandomEngine, random,
    std::vector<PlayerId>, players,
    LevelId, levelId,
    Level, level,
    boost::posix_time::time_duration, tickInterval
  > {
  Game() : base(
      LevelPack(), GameSettings(), RandomEngine(), std::vector<PlayerId>(),
      LevelId::invalid(), Level(), boost::posix_time::time_duration()
    ) {}
  Game(GameSettings&& settings) : base(
      LevelPack(), settings, RandomEngine(), std::vector<PlayerId>(),
      LevelId::invalid(), Level(), settings.get<tickInterval>()
    ) {}
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Game)

  bool started() const { return get<levelId>().valid(); }

  template<typename Players>
  void start(const Players& p, GameEventHandler&);

  void startLevel(LevelId, GameEventHandler&);
  TickResult tick(ScoreHandler&, GameEventHandler&, Moves const&);
};

template<typename Players>
void Game::start(const Players& p, GameEventHandler& handler) {
  std::random_device device;
  get<random>().seed(device);
  std::vector<PlayerId>& playerIds = get<players>();
  BOOST_FOREACH(const IdedPlayer& player, p) {
    playerIds.push_back(player.get<id>());
  }
  startLevel(get<settings>().get<level>(), handler);
}

}

#endif // NIBBLES__GAME_HPP

