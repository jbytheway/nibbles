#ifndef NIBBLES__MESSAGEBASE_HPP
#define NIBBLES__MESSAGEBASE_HPP

#include <boost/shared_ptr.hpp>

#include <nibbles/messagetype.hpp>

namespace nibbles {

class MessageBase {
  public:
    typedef boost::shared_ptr<MessageBase> Ptr;
    typedef boost::shared_ptr<MessageBase const> ConstPtr;

    static Ptr create(uint8_t const* data, size_t dataLen);
    virtual MessageType type() const = 0;
    virtual const std::string& data() const = 0;
  protected:
    MessageBase() {}
    MessageBase(const MessageBase&) {}
    ~MessageBase() {}
};

}

#endif // NIBBLES__MESSAGEBASE_HPP

