#ifndef NIBBLES__MESSAGE_HPP
#define NIBBLES__MESSAGE_HPP

#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

#include <nibbles/messagetype.hpp>
#include <nibbles/messagepayload.hpp>
#include <nibbles/messagebase.hpp>

namespace nibbles {

template<int Type>
class Message : public MessageBase {
  friend class MessageBase;
  public:
    typedef typename MessagePayload<Type>::type PayloadType;

    Message(const PayloadType& payload);

    virtual MessageType type() const { return MessageType(Type); }
    virtual const std::string& data() const { return data_; }
  private:
    PayloadType payload_;
    std::string data_; // TODO: vector<uint8_t> better?

    Message(const PayloadType& payload, uint8_t const* data, size_t dataLen);
    static Ptr internalCreate(uint8_t const* data, size_t dataLen);
};

template<int Type>
Message<Type>::Message(const PayloadType& payload) :
  payload_(payload)
{
  std::ostringstream os;
  boost::archive::text_oarchive oa(os);
  oa << payload;
  data_.assign(1, char(Type));
  data_ += os.str();
}

template<int Type>
Message<Type>::Message(
    const PayloadType& payload,
    uint8_t const* data,
    size_t dataLen
  ) :
  payload_(payload),
  data_(reinterpret_cast<char const*>(data), dataLen)
{
}

template<int Type>
MessageBase::Ptr Message<Type>::internalCreate(
    uint8_t const* data,
    size_t dataLen
  )
{
  assert(data[0] == Type);
  using namespace boost::iostreams;
  stream<array_source> dataStream(
      reinterpret_cast<char const*>(data+1), dataLen-1
    );
  boost::archive::text_iarchive ia(dataStream);
  PayloadType payload;
  ia >> payload;
  return Ptr(new Message<Type>(payload, data, dataLen));
}

}

#endif // NIBBLES__MESSAGE_HPP

