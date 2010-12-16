#include <nibbles/client/client.hpp>

#include <nibbles/client/tcpclient.hpp>

namespace nibbles { namespace client {

Client::Ptr Client::create(
    boost::asio::io_service& io,
    utility::MessageHandler& out,
    Protocol const protocol,
    std::string const& address,
    uint16_t const port
  )
{
  switch (protocol) {
    case Protocol::tcp:
      return Ptr(new TcpClient(io, out, address, port));
    default:
      NIBBLES_FATAL("protocol not supported");
  }
}

Client::Client(
    boost::asio::io_service& io,
    const Socket::Ptr& socket
  ) :
  io_(io), socket_(socket)
{
}

}}

