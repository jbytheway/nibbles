#ifndef NIBBLES__PLAYER_HPP
#define NIBBLES__PLAYER_HPP

#include <nibbles/color.hpp>

namespace nibbles {

struct Player :
  utility::DataClass<
      Player,
      std::string, name,
      Color, color
    >
{
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Player)

  // Cop-out function for the benefit of multi-index
  std::string const& name() const { return get<fields::name>(); }
};

}

#endif // NIBBLES__PLAYER_HPP

