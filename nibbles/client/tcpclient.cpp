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

  // TODO: begin read
}

void TcpClient::send(const MessageBase& message)
{
  const string& data = message.data();
  if (data.size() > Network::maxPacketLen) {
    out_.message(
        Verbosity::error,
        "TCP socket: packet too long ("+
        boost::lexical_cast<string>(data.size())+" bytes)"
      );
    return;
  }
  outgoing_ += uint8_t(data.size());
  outgoing_ += data;
  if (writing_.empty())
    startWrite();
}

void TcpClient::startWrite()
{
  writing_ += outgoing_;
  outgoing_.clear();
  if (writing_.empty())
    return;
  out_.message(
      Verbosity::info,
      "TcpClient: writing "+boost::lexical_cast<string>(writing_.size())+
      " bytes\n"
    );
  async_write(socket_, buffer(writing_), boost::bind(
        &TcpClient::handleWrite, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
}

void TcpClient::handleWrite(
    const boost::system::error_code& ec,
    const size_t bytes_transferred
  )
{
  out_.message(
      Verbosity::info,
      "TcpClient: wrote "+boost::lexical_cast<string>(bytes_transferred)+
      " bytes\n"
    );
  writing_.clear();
  if (ec) {
    out_.message(Verbosity::error, "TCP socket: "+ec.message()+"\n");
    socket_.close();
  } else if (!outgoing_.empty()) {
    startWrite();
  }
}

}}

