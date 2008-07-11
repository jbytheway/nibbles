#ifndef NIBBLES__SOCKET_HPP
#define NIBBLES__SOCKET_HPP

#include <nibbles/messagebase.hpp>

namespace nibbles {

class Socket : boost::noncopyable {
  public:
    virtual ~Socket() = 0;
    virtual void send(const MessageBase&) = 0;
};

inline Socket::~Socket() {}

}

#endif // NIBBLES__SOCKET_HPP

