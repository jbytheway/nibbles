#include <nibbles/client/tcpclient.hpp>

#include <boost/lexical_cast.hpp>

#include <nibbles/network.hpp>

using namespace std;
using namespace boost::asio;
using nibbles::utility::Verbosity;

namespace nibbles { namespace client {

TcpClient::TcpClient(
    boost::asio::io_service& io,
    utility::MessageHandler& out,
    std::string const& address,
    uint16_t const port
  ) :
  Client(io),
  TcpSocket(io, out, address, port)
{
}

void TcpClient::connect()
{
  socket_.open(ip::tcp::v4());
  socket_.connect(endpoint_);

  // TODO: begin read
}

}}

