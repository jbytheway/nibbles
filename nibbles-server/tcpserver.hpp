#ifndef NIBBLES_SERVER__TCPSERVER_HPP
#define NIBBLES_SERVER__TCPSERVER_HPP

#include <boost/asio.hpp>

#include "connection.hpp"

namespace nibbles { namespace server {

class Server;

class TcpServer {
  public:
    TcpServer(Server& server);

    void serve(const boost::asio::ip::tcp::endpoint&);
    void stop();
  private:
    Server& server_;
    boost::asio::ip::tcp::acceptor acceptor_;

    void startAccept();
    void handleAccept(
        const Connection::Ptr& newConnection,
        const boost::system::error_code& error
      );
};

}}

#endif // NIBBLES_SERVER__TCPSERVER_HPP

