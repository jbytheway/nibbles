#ifndef NIBBLES__IDEDPLAYER_HPP
#define NIBBLES__IDEDPLAYER_HPP

#include <nibbles/playerid.hpp>
#include <nibbles/clientid.hpp>

namespace nibbles {

// Player with an id
struct IdedPlayer :
  utility::DataClass<
    IdedPlayer,
    utility::InheritFrom<Player>,
    PlayerId, id,
    ClientId, clientId
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(IdedPlayer)
};

}

#endif // NIBBLES__IDEDPLAYER_HPP

