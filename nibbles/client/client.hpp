#ifndef NIBBLES_CLIENT__CLIENT_HPP
#define NIBBLES_CLIENT__CLIENT_HPP

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signal.hpp>

#include <nibbles/player.hpp>
#include <nibbles/message.hpp>
#include <nibbles/socket.hpp>
#include <nibbles/utility/messagehandler.hpp>
#include <nibbles/client/protocol.hpp>

namespace nibbles { namespace client {

class Client : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Client> Ptr;

    static Ptr create(
        boost::asio::io_service&,
        utility::MessageHandler& out,
        Protocol const,
        std::string const& address,
        uint16_t const port
      );

    virtual ~Client() = 0;

    void connect() { socket_->connect(); }
    void close() { socket_->close(); }

    template<int Type>
    void postMessage(const Message<Type>& message)
    {
      io_.post(boost::bind(&Socket::send, socket_, message));
    }
    void addPlayer(const Player& player) {
      postMessage(Message<MessageType::addPlayer>(player));
    }
    void setReadiness(const bool readiness) {
      postMessage(Message<MessageType::setReadiness>(readiness));
    }
    void turn(std::pair<PlayerId, Direction> const& t) {
      postMessage(Message<MessageType::turn>(t));
    }

    boost::signal<void (MessageBase::Ptr const&)>& messageSignal() {
      return socket_->messageSignal;
    }
    boost::signal<void ()>& terminateSignal() {
      return socket_->terminateSignal;
    }
  protected:
    Client(
        boost::asio::io_service& io,
        const Socket::Ptr& socket
      );

    boost::asio::io_service& io_;
    Socket::Ptr socket_;
};

inline Client::~Client() {}

}}

#endif // NIBBLES_CLIENT__CLIENT_HPP

