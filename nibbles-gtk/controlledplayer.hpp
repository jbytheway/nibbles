#ifndef NIBBLES_GTK__CONTROLLEDPLAYER_HPP
#define NIBBLES_GTK__CONTROLLEDPLAYER_HPP

#include <array>

#include <nibbles/player.hpp>
#include <nibbles/direction.hpp>

namespace nibbles { namespace gtk {

struct ControlledPlayer :
  utility::DataClass<
      ControlledPlayer,
      utility::InheritFrom<Player>,
      std::array<uint32_t, Direction::max>, controls
    >
{
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(ControlledPlayer)
};

}}

#endif // NIBBLES_GTK__CONTROLLEDPLAYER_HPP

