#include <nibbles/tcpsocket.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <nibbles/network.hpp>
#include <nibbles/utility/ntoh.hpp>
#include <nibbles/utility/hton.hpp>

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
  auto const addr = boost::asio::ip::address::from_string(address);
  endpoint_ = boost::asio::ip::tcp::endpoint(addr, port);
  out_.message(
      utility::Verbosity::info, "tcp: connecting to "+addr.to_string()+":"+
      boost::lexical_cast<std::string>(port)
    );
}

void TcpSocket::connect()
{
  socket_.open(boost::asio::ip::tcp::v4());
  socket_.connect(endpoint_);

  continueRead();
}

void TcpSocket::send(const MessageBase& message)
{
  const auto& data = message.data();
  if (data.size() > Network::maxPacketLen) {
    out_.message(
        utility::Verbosity::error,
        "TCP socket: packet too long ("+
        boost::lexical_cast<std::string>(data.size())+" bytes)"
      );
    return;
  }
  Network::PacketLength length = data.size();
  utility::hton(length);
  outgoing_ += std::string(reinterpret_cast<char*>(&length), sizeof(length));
  outgoing_ += data;
  if (writing_.empty())
    startWrite();
}

void TcpSocket::continueRead()
{
  socket_.async_read_some(
      boost::asio::buffer(data_.data()+dataLen_, data_.size()-dataLen_),
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
  if (error) {
    out_.message(
      utility::Verbosity::error, "TcpSocket: read: "+error.message()
    );
    terminateSignal();
  } else {
    dataLen_ += bytes;
    while (true) {
      Network::PacketLength packetLength;
      if (dataLen_ < sizeof(packetLength)) break;
      memcpy(&packetLength, data_.data(), sizeof(packetLength));
      utility::ntoh(packetLength);
      if (dataLen_ < sizeof(packetLength)+packetLength) break;
      uint8_t const* const packetStart = data_.data()+sizeof(packetLength);
      messageSignal(MessageBase::create(packetStart, packetLength));
      memmove(
        data_.data(), packetStart + packetLength,
        dataLen_ - packetLength - sizeof(packetLength)
      );
      dataLen_ -= (packetLength + sizeof(packetLength));
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
  boost::asio::async_write(socket_, boost::asio::buffer(writing_), boost::bind(
        &TcpSocket::handleWrite, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
}

void TcpSocket::handleWrite(
    const boost::system::error_code& ec,
    const size_t /*bytes_transferred*/
  )
{
  // TODO: use bytes_transferred
  writing_.clear();
  if (ec) {
    out_.message(utility::Verbosity::error, "TCP socket: "+ec.message());
    socket_.close();
  } else if (!outgoing_.empty()) {
    startWrite();
  }
}

}

