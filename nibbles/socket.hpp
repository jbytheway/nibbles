#ifndef NIBBLES__SOCKET_HPP
#define NIBBLES__SOCKET_HPP

#include <boost/signal.hpp>

#include <nibbles/messagebase.hpp>

namespace nibbles {

class Socket : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Socket> Ptr;
    virtual ~Socket() = 0;
    virtual void connect() = 0; // When not a listening end
    virtual void read() = 0; // When a listening end
    virtual void send(const MessageBase&) = 0;
    virtual void close() = 0;

    boost::signal<void (MessageBase::Ptr const&)> messageSignal;
    boost::signal<void ()> terminateSignal;
};

inline Socket::~Socket() {}

}

#endif // NIBBLES__SOCKET_HPP

