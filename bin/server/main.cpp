#include <boost/asio.hpp>

#include "server.hpp"
#include "optionserror.hpp"
#include "signalcatcher.hpp"

static void signalHandler(int signal)
{
  nibbles::server::signalCatcher(signal);
}

int main(int argc, char const* const* const argv)
{
  try {
    boost::asio::io_service io;
    nibbles::server::Options options(argc, argv);
    nibbles::server::Server server(io, std::cout, options);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    server.serve();
  } catch (nibbles::server::OptionsError& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}

