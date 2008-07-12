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
#define CASE(r, d, value)                                                \
    case MessageType::value:                                             \
      return Message<MessageType::value>::internalCreate(data, dataLen);
    BOOST_PP_SEQ_FOR_EACH(CASE, _, NIBBLES_MESSAGETYPE_VALUES())
#undef CASE
    default:
      throw DeserializationException("unknown MessageType");
  }
}

}

