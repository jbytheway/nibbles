#include "tcpserver.hpp"

#include "server.hpp"
#include "tcpconnection.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::system;
using namespace nibbles::utility;

namespace nibbles { namespace server {

TcpServer::TcpServer(Server& server) :
  server_(server),
  acceptor_(server.io())
{}

void TcpServer::serve(const ip::tcp::endpoint& ep)
{
  acceptor_.open(ip::tcp::v4());
  startBind(ep);
}

void TcpServer::stop()
{
  server_.message(Verbosity::info, "TCP server: stopping\n");
  acceptor_.close();
}

void TcpServer::startBind(const ip::tcp::endpoint& ep)
{
  server_.message(
      Verbosity::info,
      "TCP server: binding to "+ep.address().to_string()+":"+
      boost::lexical_cast<string>(ep.port())+"\n"
    );
  error_code error;
  acceptor_.bind(ep, error);
  if (error == posix_error::address_in_use) {
    server_.message(
        Verbosity::error, "TCP server: address in use, will retry later\n"
      );
    deadline_timer t(server_.io(), boost::posix_time::seconds(5));
    t.async_wait(boost::bind(&TcpServer::startBind, this, ep));
  } else if (error) {
    server_.message(
        Verbosity::error, "TCP server: bind failed: "+error.message()+"\n"
      );
  } else {
    acceptor_.listen();
    startAccept();
  }
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
    server_.message(
        Verbosity::error, "TCP server: accept: "+error.message()+"\n"
      );
  } else {
    server_.message(Verbosity::info, "TCP server: accepted connection");
    server_.addConnection(newConnection);
    startAccept();
  }
}

}}

