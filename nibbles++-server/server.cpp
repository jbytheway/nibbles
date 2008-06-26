#include "server.hpp"

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

namespace nibbles { namespace server {

Server::Server(io_service& io_, ostream& out_, const Options& o) :
  io(io_),
  out(out_),
  options(o),
  tcpAcceptor(io)
{
  if (options.useTcp) {
    ip::address addr(ip::address::from_string(options.tcpAddress));
    tcpAcceptor.open(tcp::v4());
    tcpAcceptor.bind(tcp::endpoint(addr, options.tcpPort));
  }
}

void Server::serve()
{
  io.run(); // Blocks until all stuff is done
  writeLine(Verbosity::info, "server done\n");
}

void Server::writeLine(Verbosity v, const string& message)
{
  if (options.verbosity <= v)
    out << message;
}

}}

