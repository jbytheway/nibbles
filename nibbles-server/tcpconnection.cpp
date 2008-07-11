#include "tcpconnection.hpp"

#include "server.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost::asio;
using namespace nibbles::utility;

namespace nibbles { namespace server {

TcpConnection::TcpConnection(Server& server) :
  TcpSocket(server.io(), server),
  server_(server),
  dataLen_(0)
{}

void TcpConnection::start()
{
  server_.message(Verbosity::info, "TcpConnection::start()\n");
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
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred,
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
  server_.message(
      Verbosity::info, "read "+boost::lexical_cast<string>(bytes)+" bytes\n"
    );
  if (error) {
    server_.message(Verbosity::error, "read: "+error.message()+"\n");
    terminateSignal(this);
  } else {
    dataLen_ += bytes;
    size_t packetLen;
    while (dataLen_ >= 1+(packetLen = data_[0])) {
      server_.message(Verbosity::info, "got packet\n");
      uint8_t const* const packetStart = data_.data()+1;
      messageSignal(*MessageBase::create(packetStart, packetLen), this);
      memmove(data_.data(), packetStart+packetLen, dataLen_-packetLen-1);
      dataLen_ -= (packetLen + 1);
    }
    continueRead();
  }
}

}}

