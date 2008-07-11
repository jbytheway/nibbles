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
  template<typename... Args>
  explicit IdedPlayer(Args&&... args) : base(args...) {}
};

}

#endif // NIBBLES__IDEDPLAYER_HPP

