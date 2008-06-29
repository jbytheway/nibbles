#include "tcpconnection.hpp"

#include "server.hpp"

#include <boost/bind.hpp>

using namespace boost::asio;
using namespace nibbles::utility;

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

void TcpConnection::close()
{
  // TODO use shutdown?
  socket_.close();
}

void TcpConnection::continueRead()
{
  socket_.async_read_some(
      buffer(data_.data()+dataLen_, data_.size()-dataLen_),
      boost::bind(
        &TcpConnection::handleRead, this,
        placeholders::error, placeholders::bytes_transferred,
        // send shared pointer to self through async call to ensure that not
        // destructed until call is completed
        ptrToThis_.lock()
      )
    );
}

void TcpConnection::handleRead(
    const boost::system::error_code& error,
    std::size_t bytes,
    const Ptr&
  )
{
  if (error) {
    server_.message(Verbosity::error, "read: "+error.message()+"\n");
    terminateSignal(this);
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

