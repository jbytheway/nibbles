#ifndef NIBBLES__MESSAGEBASE_HPP
#define NIBBLES__MESSAGEBASE_HPP

namespace nibbles {

class MessageBase {
  public:
    virtual const std::string& data() const = 0;
  protected:
    MessageBase() {}
    MessageBase(const MessageBase&) {}
    ~MessageBase() {}
};

}

#endif // NIBBLES__MESSAGEBASE_HPP

