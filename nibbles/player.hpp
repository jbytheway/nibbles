#ifndef NIBBLES__PLAYER_HPP
#define NIBBLES__PLAYER_HPP

#include <nibbles/color.hpp>

namespace nibbles {

class Player :
  public utility::DataClass<
      std::string, name,
      Color, color
    >
{
};

}

#endif // NIBBLES__PLAYER_HPP

