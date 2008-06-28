#ifndef NIBBLES_GTK__OPTIONS_HPP
#define NIBBLES_GTK__OPTIONS_HPP

#include <string>

#include <nibbles/client/protocol.hpp>

namespace nibbles { namespace gtk {

struct Options {
  Options(int const argc, char** const argv);

  client::Protocol protocol;
  std::string address;
  uint16_t port;
};

}}

#endif // NIBBLES_GTK__OPTIONS_HPP

