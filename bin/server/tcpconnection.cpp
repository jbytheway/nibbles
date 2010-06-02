#include "tcpconnection.hpp"

#include "server.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost::asio;
using namespace nibbles::utility;

namespace nibbles { namespace server {

TcpConnection::TcpConnection(Server& server) :
  Connection(TcpSocket::create(server.io(), server))
{}

}}

