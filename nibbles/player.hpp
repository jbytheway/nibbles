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
  template<typename... Args>
  explicit Player(Args&&... args) : base(args...) {}
};

}

#endif // NIBBLES__PLAYER_HPP

