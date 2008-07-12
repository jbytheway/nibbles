#ifndef NIBBLES_SERVER__TCPCONNECTION_HPP
#define NIBBLES_SERVER__TCPCONNECTION_HPP

#include <boost/weak_ptr.hpp>

#include <nibbles/network.hpp>
#include <nibbles/tcpsocket.hpp>

#include "connection.hpp"

namespace nibbles { namespace server {

class Server;

class TcpConnection : public Connection {
  public:
    typedef boost::shared_ptr<TcpConnection> Ptr;

    TcpConnection(Server& server);
    virtual ~TcpConnection() {}

    boost::asio::ip::tcp::socket& socket() {
      return boost::dynamic_pointer_cast<TcpSocket>(socket_)->socket();
    }
  private:
};

}}

#endif // NIBBLES_SERVER__TCPCONNECTION_HPP

