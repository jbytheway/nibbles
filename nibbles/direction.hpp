#ifndef NIBBLES__DIRECTION_HPP
#define NIBBLES__DIRECTION_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles { namespace utility {

#define NIBBLES_DIRECTION_VALUES() (up)(down)(left)(right)
NIBBLES_UTILITY_ENUM_HEADER(Direction, NIBBLES_DIRECTION_VALUES())
  
}}

#endif // NIBBLES__DIRECTION_HPP

