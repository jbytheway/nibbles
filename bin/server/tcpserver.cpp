#include "tcpserver.hpp"

#include "server.hpp"
#include "tcpconnection.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace nibbles { namespace server {

TcpServer::TcpServer(Server& server) :
  server_(server),
  acceptor_(server.io()),
  bindTimer_(server.io())
{}

void TcpServer::serve(const boost::asio::ip::tcp::endpoint& ep)
{
  acceptor_.open(boost::asio::ip::tcp::v4());
  startBind(ep, boost::system::error_code());
}

void TcpServer::stop()
{
  server_.message(utility::Verbosity::info, "TCP server: stopping");
  acceptor_.close();
  bindTimer_.cancel();
}

void TcpServer::startBind(
    const boost::asio::ip::tcp::endpoint& ep,
    const boost::system::error_code& ec
  )
{
  if (ec) {
    server_.message(
        utility::Verbosity::error,
        "TCP server: when starting bind: "+ec.message()
      );
    return;
  }
  server_.message(
      utility::Verbosity::info,
      "TCP server: binding to "+ep.address().to_string()+":"+
      boost::lexical_cast<std::string>(ep.port())
    );
  boost::system::error_code error;
  acceptor_.bind(ep, error);
  if (error == boost::system::posix_error::address_in_use) {
    server_.message(
        utility::Verbosity::error,
        "TCP server: address in use, will retry later"
      );
    bindTimer_.expires_from_now(boost::posix_time::seconds(5));
    bindTimer_.async_wait(boost::bind(
          &TcpServer::startBind, this, ep, boost::asio::placeholders::error
        ));
  } else if (error) {
    server_.message(
        utility::Verbosity::error, "TCP server: bind failed: "+error.message()
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
        utility::Verbosity::error, "TCP server: accept: "+error.message()
      );
  } else {
    server_.message(
      utility::Verbosity::info, "TCP server: accepted connection"
    );
    server_.addConnection(newConnection);
    startAccept();
  }
}

}}

