#include <boost/asio.hpp>

#include "server.hpp"
#include "signalcatcher.hpp"

using namespace std;
using namespace boost::asio;
using namespace nibbles::server;

static void signalHandler(int signal)
{
  signalCatcher(signal);
}

int main(int argc, char const* const* const argv)
{
  io_service io;
  Options options(argc, argv);
  Server server(io, cout, options);
  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);
  server.serve();
  return 0;
}

