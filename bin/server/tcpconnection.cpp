#include "tcpconnection.hpp"

#include "server.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace nibbles { namespace server {

TcpConnection::TcpConnection(Server& server) :
  Connection(TcpSocket::create(server.io(), server))
{}

}}

