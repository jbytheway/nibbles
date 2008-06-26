#include "options.hpp"

namespace nibbles { namespace server {

Options::Options(int /*argc*/, char const* const* const /*argv*/) :
  verbosity(Verbosity::info),
  useTcp(true),
  tcpAddress("127.0.0.1"),
  tcpPort(53788)
{
}

}}

