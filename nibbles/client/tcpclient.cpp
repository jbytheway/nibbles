#include <nibbles/client/tcpclient.hpp>

#include <boost/lexical_cast.hpp>

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
  io_(io),
  out_(out),
  socket_(io)
{
  ip::address addr = ip::address::from_string(address);
  endpoint_ = ip::tcp::endpoint(addr, port);
  out_.message(
      Verbosity::info, "tcp: connecting to "+addr.to_string()+":"+
      boost::lexical_cast<string>(port)+"\n"
    );
}

void TcpClient::connect()
{
  socket_.open(ip::tcp::v4());
  socket_.connect(endpoint_);

  // TODO: begin write
}

}}

