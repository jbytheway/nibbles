#include "options.hpp"

#include <cstdlib>
#include <nibbles/network.hpp>

using namespace std;
using namespace nibbles::utility;
using namespace nibbles::client;

namespace nibbles { namespace gtk {

Options::Options(int const /*argc*/, char** const /*argv*/) :
  verbosity(Verbosity::info),
  threaded(false),
  playerFile(string(getenv("HOME"))+"/.nibbles/players"),
  protocol(Protocol::tcp),
  address("127.0.0.1"),
  port(Network::defaultPort)
{
  // TODO: parse options
}

}}

