#include <nibbles/client/tcpclient.hpp>

#include <boost/lexical_cast.hpp>

#include <nibbles/network.hpp>

namespace nibbles { namespace client {

TcpClient::TcpClient(
    boost::asio::io_service& io,
    utility::MessageHandler& out,
    std::string const& address,
    uint16_t const port
  ) :
  Client(io, TcpSocket::create(io, out, address, port))
{
}

}}

