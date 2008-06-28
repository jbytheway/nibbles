#include <nibbles/client/tcpclient.hpp>

using namespace boost::asio;

namespace nibbles { namespace client {

TcpClient::TcpClient(
    boost::asio::io_service& io,
    std::string const& address,
    uint16_t const port
  ) :
  io_(io),
  socket_(io)
{
  ip::address addr = ip::address::from_string(address);
  endpoint_ = ip::tcp::endpoint(addr, port);
}

void TcpClient::connect()
{
  socket_.open(ip::tcp::v4());
  socket_.connect(endpoint_);

  // TODO: begin write
}

}}

