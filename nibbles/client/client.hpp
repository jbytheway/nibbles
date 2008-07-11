#ifndef NIBBLES_CLIENT__CLIENT_HPP
#define NIBBLES_CLIENT__CLIENT_HPP

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <nibbles/player.hpp>
#include <nibbles/message.hpp>
#include <nibbles/socket.hpp>
#include <nibbles/utility/messagehandler.hpp>
#include <nibbles/client/protocol.hpp>

namespace nibbles { namespace client {

class Client : public virtual Socket {
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

    virtual void connect() = 0;
    template<int Type>
    void postMessage(const Message<Type>& message)
    {
      io_.post(boost::bind(&Client::send, this, message));
    }
    void addPlayer(const Player&);
  protected:
    Client(boost::asio::io_service& io) : io_(io) {}

    boost::asio::io_service& io_;
};

inline Client::~Client() {}

}}

#endif // NIBBLES_CLIENT__CLIENT_HPP

