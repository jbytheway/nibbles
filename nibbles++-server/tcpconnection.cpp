#include "tcpconnection.hpp"

#include "server.hpp"

#include <boost/bind.hpp>

using namespace boost::asio;

namespace nibbles { namespace server {

TcpConnection::TcpConnection(Server& server) :
  server_(server),
  socket_(server.io()),
  dataLen(0),
  returnPath(*this)
{}

void TcpConnection::start()
{
  continueRead();
}

void TcpConnection::continueRead()
{
  socket_.async_read_some(
      buffer(data.data()+dataLen, data.size()-dataLen),
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
    dataLen += bytes;
    size_t packetLen;
    while (dataLen >= 1+(packetLen = data[0])) {
      uint8_t const* const packetStart = data.data()+1;
      packetSignal(Packet(packetStart, packetLen), returnPath);
      memmove(data.data(), packetStart+packetLen, dataLen-packetLen);
    }
    continueRead();
  }
}

}}

