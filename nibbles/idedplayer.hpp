#ifndef NIBBLES__IDEDPLAYER_HPP
#define NIBBLES__IDEDPLAYER_HPP

#include <nibbles/playerid.hpp>

namespace nibbles {

// Player with an id
struct IdedPlayer :
  utility::DataClass<
    IdedPlayer,
    utility::InheritFrom<Player>,
    PlayerId, id
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(IdedPlayer)
};

}

#endif // NIBBLES__IDEDPLAYER_HPP

