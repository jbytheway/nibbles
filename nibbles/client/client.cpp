#include <nibbles/client/client.hpp>

#include <nibbles/client/tcpclient.hpp>

using namespace std;

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
      return Client::Ptr(new TcpClient(io, out, address, port));
    default:
      throw logic_error("protocol not supported");
  }
}

}}

