#include <nibbles/client/client.hpp>

#include <nibbles/message.hpp>
#include <nibbles/client/tcpclient.hpp>

using namespace std;

namespace nibbles { namespace client {

Client::Ptr Client::create(
    boost::asio::io_service& io,
    utility::MessageHandler& out,
    Protocol const protocol,
    std::string const& address,
    uint16_t const port
  )
{
  switch (protocol) {
    case Protocol::tcp:
      return Ptr(new TcpClient(io, out, address, port));
    default:
      throw logic_error("protocol not supported");
  }
}

Client::Client(
    boost::asio::io_service& io,
    const Socket::Ptr& socket
  ) :
  io_(io), socket_(socket)
{
  socket_->messageSignal.connect(
      boost::bind(boost::ref(messageSignal), _1, this)
    );
  socket_->terminateSignal.connect(
      boost::bind(boost::ref(terminateSignal), this)
    );
  // TODO: should disconnect these on destruction
}

void Client::addPlayer(const Player& player)
{
  postMessage(Message<MessageType::addPlayer>(player));
}

}}

