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
  template<typename... Args>
  explicit ControlledPlayer(Args&&... args) : base(args...) {}
};

}}

#endif // NIBBLES_GTK__CONTROLLEDPLAYER_HPP

