#ifndef NIBBLES__MESSAGEPAYLOAD_HPP
#define NIBBLES__MESSAGEPAYLOAD_HPP

#include <nibbles/player.hpp>
#include <nibbles/idedplayer.hpp>
#include <nibbles/clientid.hpp>
#include <nibbles/leveldefinition.hpp>
#include <nibbles/number.hpp>
#include <nibbles/moves.hpp>

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

template<>
struct MessagePayload<MessageType::playerAdded> {
  typedef IdedPlayer type;
};

template<>
struct MessagePayload<MessageType::setReadiness> {
  typedef bool type;
};

template<>
struct MessagePayload<MessageType::updateReadiness> {
  typedef std::pair<ClientId, bool> type;
};

template<>
struct MessagePayload<MessageType::gameStart> {
  // TODO: should really be nothing, but can't be bothered to make that work
  typedef int type;
};

template<>
struct MessagePayload<MessageType::levelStart> {
  typedef LevelDefinition type;
};

template<>
struct MessagePayload<MessageType::newNumber> {
  typedef Number type;
};

template<>
struct MessagePayload<MessageType::tick> {
  typedef Moves type;
};

}

#endif // NIBBLES__MESSAGEPAYLOAD_HPP

