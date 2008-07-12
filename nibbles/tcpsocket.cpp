#include <nibbles/tcpsocket.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <nibbles/network.hpp>

using namespace std;
using namespace boost::asio;
using namespace nibbles::utility;

namespace nibbles {

TcpSocket::TcpSocket(
    boost::asio::io_service& io,
    utility::MessageHandler& out
  ) :
  out_(out),
  socket_(io),
  dataLen_(0)
{
}

TcpSocket::TcpSocket(
    boost::asio::io_service& io,
    utility::MessageHandler& out,
    std::string const& address,
    uint16_t const port
  ) :
  out_(out),
  socket_(io),
  dataLen_(0)
{
  ip::address addr = ip::address::from_string(address);
  endpoint_ = ip::tcp::endpoint(addr, port);
  out_.message(
      Verbosity::info, "tcp: connecting to "+addr.to_string()+":"+
      boost::lexical_cast<string>(port)+"\n"
    );
}

void TcpSocket::connect()
{
  socket_.open(ip::tcp::v4());
  socket_.connect(endpoint_);

  continueRead();
}

void TcpSocket::send(const MessageBase& message)
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

void TcpSocket::continueRead()
{
  socket_.async_read_some(
      buffer(data_.data()+dataLen_, data_.size()-dataLen_),
      boost::bind(
        &TcpSocket::handleRead, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred,
        // send shared pointer to self through async call to ensure that not
        // destructed until call is completed
        ptrToThis_.lock()
      )
    );
}

void TcpSocket::handleRead(
    const boost::system::error_code& error,
    std::size_t bytes,
    const Ptr&
  )
{
  out_.message(
      Verbosity::info, "read "+boost::lexical_cast<string>(bytes)+" bytes\n"
    );
  if (error) {
    out_.message(Verbosity::error, "read: "+error.message()+"\n");
    terminateSignal();
  } else {
    dataLen_ += bytes;
    size_t packetLen;
    while (dataLen_ >= 1+(packetLen = data_[0])) {
      out_.message(Verbosity::info, "got packet\n");
      uint8_t const* const packetStart = data_.data()+1;
      messageSignal(MessageBase::create(packetStart, packetLen));
      memmove(data_.data(), packetStart+packetLen, dataLen_-packetLen-1);
      dataLen_ -= (packetLen + 1);
    }
    continueRead();
  }
}

void TcpSocket::startWrite()
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
        &TcpSocket::handleWrite, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
}

void TcpSocket::handleWrite(
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

}

