#include "options.hpp"

#include <nibbles/network.hpp>

using namespace nibbles::utility;

namespace nibbles { namespace server {

Options::Options(int /*argc*/, char const* const* const /*argv*/) :
  verbosity(Verbosity::debug),
  useTcp(true),
  tcpAddress("127.0.0.1"),
  tcpPort(Network::defaultPort)
{
}

}}

