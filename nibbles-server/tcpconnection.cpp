#include "tcpconnection.hpp"

#include "server.hpp"

#include <boost/bind.hpp>

using namespace boost::asio;

namespace nibbles { namespace server {

TcpConnection::TcpConnection(Server& server) :
  server_(server),
  socket_(server.io()),
  dataLen_(0),
  returnPath_(*this)
{}

void TcpConnection::start()
{
  continueRead();
}

void TcpConnection::continueRead()
{
  socket_.async_read_some(
      buffer(data_.data()+dataLen_, data_.size()-dataLen_),
      boost::bind(
        &TcpConnection::handleRead, this,
        placeholders::error, placeholders::bytes_transferred
      )
    );
}

void TcpConnection::handleRead(
    const boost::system::error_code& error,
    std::size_t bytes
  )
{
  if (error) {
    server_.message(Verbosity::error, "read: "+error.message()+"\n");
    // TODO delete myself somehow
  } else {
    dataLen_ += bytes;
    size_t packetLen;
    while (dataLen_ >= 1+(packetLen = data_[0])) {
      uint8_t const* const packetStart = data_.data()+1;
      packetSignal(Packet(packetStart, packetLen), returnPath_);
      memmove(data_.data(), packetStart+packetLen, dataLen_-packetLen-1);
    }
    continueRead();
  }
}

}}

