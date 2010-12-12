#ifndef NIBBLES_GTK__OPTIONS_HPP
#define NIBBLES_GTK__OPTIONS_HPP

#include <string>

#include <boost/filesystem/path.hpp>

#include <nibbles/utility/verbosity.hpp>
#include <nibbles/client/protocol.hpp>

namespace nibbles { namespace gtk {

struct Options {
  Options(int const argc, char** const argv);

  bool help;
  utility::Verbosity verbosity;
  bool threaded;
  std::string playerFile;
  client::Protocol protocol;
  std::string address;
  uint16_t port;
  boost::filesystem::path gladePath;
  boost::filesystem::path fontPath;

  void show_help(std::ostream&) const;
};

}}

#endif // NIBBLES_GTK__OPTIONS_HPP

