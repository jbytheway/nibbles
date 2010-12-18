#ifndef NIBBLES_GTK__OPTIONS_HPP
#define NIBBLES_GTK__OPTIONS_HPP

#include <string>

#include <boost/filesystem/path.hpp>

#include <nibbles/utility/verbosity.hpp>
#include <nibbles/client/protocol.hpp>

namespace nibbles { namespace gtk {

struct Options {
  Options(int const argc, char** const argv);

  std::string address;
  boost::filesystem::path fontPath;
  boost::filesystem::path gladePath;
  bool help;
  uint16_t port;
  client::Protocol protocol;
  boost::filesystem::path soundPath;
  bool threaded;
  utility::Verbosity verbosity;
  std::string playerFile;

  void show_help(std::ostream&) const;
};

}}

#endif // NIBBLES_GTK__OPTIONS_HPP

