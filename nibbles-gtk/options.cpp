#include "options.hpp"

#include <nibbles/network.hpp>

using namespace nibbles::client;

namespace nibbles { namespace gtk {

Options::Options(int const /*argc*/, char** const /*argv*/) :
  protocol(Protocol::tcp),
  address("127.0.0.1"),
  port(Network::defaultPort)
{
  // TODO: parse options
}

}}

