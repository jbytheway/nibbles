#ifndef NIBBLES__GAMESETTINGS_HPP
#define NIBBLES__GAMESETTINGS_HPP

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>
#include <nibbles/levelid.hpp>
#include <nibbles/snakelength.hpp>
#include <nibbles/lifecount.hpp>
#include <nibbles/score.hpp>

namespace nibbles {

struct GameSettings :
  utility::DataClass<
    GameSettings,
    LevelId, startLevel,
    boost::posix_time::time_duration, tickInterval,
    double, tickIntervalFactor,
    LifeCount, startLives,
    SnakeLength, startLength,
    SnakeLength, growthFactor,
    Score, deathScore
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(GameSettings)
};

// Ordering doesn't mean anything in particular; just so that GameSettings can
// be used as an AssociativeContainer key
bool operator<(GameSettings const&, GameSettings const&);

}

#endif // NIBBLES__GAMESETTINGS_HPP

