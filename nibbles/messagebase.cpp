#include <nibbles/messagebase.hpp>

#include <stdexcept>

#include <nibbles/message.hpp>
#include <nibbles/messagetype.hpp>
#include <nibbles/deserializationexception.hpp>

using namespace std;

namespace nibbles {

MessageBase::Ptr MessageBase::create(uint8_t const* data, size_t dataLen)
{
  if (dataLen == 0)
    throw DeserializationException("empty message");
  switch (data[0]) {
    case MessageType::addPlayer:
      return Message<MessageType::addPlayer>::internalCreate(data, dataLen);
    default:
      throw DeserializationException("unknown MessageType");
  }
}

}

