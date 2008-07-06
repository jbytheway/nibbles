#ifndef NIBBLES__MESSAGEPAYLOAD_HPP
#define NIBBLES__MESSAGEPAYLOAD_HPP

#include <nibbles/player.hpp>

namespace nibbles {

template<int Type>
struct MessagePayload {
  static_assert(
      Type == 0 && Type == 1,
      "invalid Type, you should be using one of the explicit specializations"
    );
};

template<>
struct MessagePayload<MessageType::addPlayer> {
  typedef Player type;
};

}

#endif // NIBBLES__MESSAGEPAYLOAD_HPP

