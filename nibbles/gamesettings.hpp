#ifndef NIBBLES__GAMESETTINGS_HPP
#define NIBBLES__GAMESETTINGS_HPP

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>
#include <nibbles/levelid.hpp>
#include <nibbles/snakelength.hpp>

namespace nibbles {

struct GameSettings :
  utility::DataClass<
    GameSettings,
    LevelId, level,
    boost::posix_time::time_duration, tickInterval,
    SnakeLength, startLength
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(GameSettings)
};

}

#endif // NIBBLES__GAMESETTINGS_HPP

