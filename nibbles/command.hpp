#ifndef NIBBLES__COMMAND_HPP
#define NIBBLES__COMMAND_HPP

#include <nibbles/direction.hpp>

namespace nibbles {

#define NIBBLES_COMMAND_VALUES() \
  NIBBLES_DIRECTION_VALUES()(pause)
NIBBLES_UTILITY_ENUM_HEADER(Command, NIBBLES_COMMAND_VALUES())

inline Command& operator++(Command& c)
{
  c = Command(c+1);
  return c;
}

}

#endif // NIBBLES__COMMAND_HPP

