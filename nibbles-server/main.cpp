#include <boost/asio.hpp>

#include "server.hpp"

using namespace std;
using namespace boost::asio;
using namespace nibbles::server;

int main(int argc, char const* const* const argv)
{
  io_service io;
  Options options(argc, argv);
  Server server(io, cout, options);
  server.serve();
  return 0;
}

