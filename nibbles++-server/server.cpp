#include "server.hpp"

#include <boost/bind.hpp>

using namespace std;
using namespace boost::asio;

namespace nibbles { namespace server {

Server::Server(io_service& io, ostream& out, const Options& o) :
  io_(io),
  out_(out),
  options_(o),
  tcp_(*this)
{
}

void Server::serve()
{
  if (options_.useTcp) {
    ip::address addr(ip::address::from_string(options_.tcpAddress));
    tcp_.serve(ip::tcp::endpoint(addr, options_.tcpPort));
  }

  io_.run(); // Blocks until all stuff is done
  message(Verbosity::info, "server done\n");
}

void Server::addConnection(const Connection::Ptr& connection)
{
  connection->packetSignal.connect(
      boost::bind(&Server::packet, this, _1, _2)
    );
  connection->start();
  connectionPool_.insert(connection);
}

void Server::message(Verbosity v, const string& message)
{
  if (options_.verbosity <= v)
    out_ << message;
}

void Server::packet(const Packet&, const ReturnPath&)
{
  abort();
}

}}

