#ifndef NIBBLES_SERVER__OPTIONS_HPP
#define NIBBLES_SERVER__OPTIONS_HPP

#include <string>

#include <nibbles/utility/verbosity.hpp>

namespace nibbles { namespace server {

struct Options
{
  Options(int argc, char const* const* const argv);

  utility::Verbosity verbosity;
  bool useTcp;
  std::string tcpAddress;
  uint16_t tcpPort;
};

}}

#endif // NIBBLES_SERVER__OPTIONS_HPP

