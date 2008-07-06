#ifndef NIBBLES__MESSAGE_HPP
#define NIBBLES__MESSAGE_HPP

#include <sstream>

#include <boost/archive/text_oarchive.hpp>

#include <nibbles/messagetype.hpp>
#include <nibbles/messagepayload.hpp>
#include <nibbles/messagebase.hpp>

namespace nibbles {

template<int Type>
class Message : public MessageBase {
  public:
    typedef typename MessagePayload<Type>::type PayloadType;

    Message(const PayloadType& payload);

    virtual const std::string& data() const { return data_; }
  private:
    PayloadType payload_;
    std::string data_;
};

template<int Type>
Message<Type>::Message(const PayloadType& payload) :
  payload_(payload)
{
  std::ostringstream os;
  boost::archive::text_oarchive oa(os);
  oa << payload;
  data_ = os.str();
}

}

#endif // NIBBLES__MESSAGE_HPP

