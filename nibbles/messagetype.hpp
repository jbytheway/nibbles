#ifndef NIBBLES__MESSAGETYPE_HPP
#define NIBBLES__MESSAGETYPE_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles {

#define NIBBLES_MESSAGETYPE_VALUES() \
  (addPlayer)(playerAdded)(setReadiness)(updateReadiness)\
  (gameStart)(levelStart)(newNumber)(tick)

NIBBLES_UTILITY_ENUM_HEADER(MessageType, NIBBLES_MESSAGETYPE_VALUES())

}

#endif // NIBBLES__MESSAGETYPE_HPP

