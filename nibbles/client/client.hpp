#ifndef NIBBLES_CLIENT__CLIENT_HPP
#define NIBBLES_CLIENT__CLIENT_HPP

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <nibbles/client/protocol.hpp>
#include <nibbles/utility/messagehandler.hpp>

namespace nibbles { namespace client {

class Client : private boost::noncopyable {
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
  protected:
    Client() {}
};

inline Client::~Client() {}

}}

#endif // NIBBLES_CLIENT__CLIENT_HPP

