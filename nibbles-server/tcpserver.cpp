#include "tcpserver.hpp"

#include "server.hpp"
#include "tcpconnection.hpp"

#include <boost/bind.hpp>

using namespace boost::asio;

namespace nibbles { namespace server {

TcpServer::TcpServer(Server& server) :
  server_(server),
  acceptor_(server.io())
{}

void TcpServer::serve(const ip::tcp::endpoint& ep)
{
  acceptor_.open(ip::tcp::v4());
  server_.message(Verbosity::info, "TCP server: binding\n");
  acceptor_.bind(ep);
  acceptor_.listen();

  startAccept();
}

void TcpServer::stop()
{
  acceptor_.close();
}

void TcpServer::startAccept()
{
  TcpConnection::Ptr newConnection(new TcpConnection(server_));

  acceptor_.async_accept(
      newConnection->socket(),
      boost::bind(&TcpServer::handleAccept, this, newConnection,
        boost::asio::placeholders::error)
    );
}

void TcpServer::handleAccept(
    const Connection::Ptr& newConnection,
    const boost::system::error_code& error
  )
{
  if (error) {
    server_.message(Verbosity::error, "accept: "+error.message()+"\n");
  } else {
    server_.addConnection(newConnection);
    startAccept();
  }
}

}}

