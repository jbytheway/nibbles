#include "server.hpp"

#include "signalcatcher.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <nibbles/utility/nulldeleter.hpp>

using namespace std;
using namespace boost::asio;
using namespace nibbles::utility;

namespace nibbles { namespace server {

Server::Server(io_service& io, ostream& out, const Options& o) :
  io_(io),
  out_(out),
  options_(o),
  tcp_(*this)
{
  signalCatcher.connect(boost::bind(&Server::shutdown, this));
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
  connection->terminateSignal.connect(
      boost::bind(&Server::deleteConnection, this, _1)
    );
  connection->start();
  connectionPool_.insert(connection);
  message(
      Verbosity::info,
      "added connection; "+
      boost::lexical_cast<string>(connectionPool_.size())+" now exist\n"
    );
}

void Server::message(Verbosity v, const string& message)
{
  if (options_.verbosity <= v)
    out_ << message << flush;
}

void Server::packet(const Packet&, const ReturnPath&)
{
  // TODO: deal
  abort();
}

void Server::shutdown()
{
  message(Verbosity::info, "caught interrupt, shutting down...\n");
  tcp_.stop();
  connectionPool_.clear();
}

void Server::deleteConnection(Connection* connection)
{
  Connection::Ptr fakePtr(connection, NullDeleter());
  connectionPool_.erase(fakePtr);
  message(
      Verbosity::info,
      "removed connection; "+
      boost::lexical_cast<string>(connectionPool_.size())+" remain\n"
    );
}

}}

