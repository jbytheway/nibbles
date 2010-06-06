#ifndef NIBBLES_SERVER__OPTIONS_HPP
#define NIBBLES_SERVER__OPTIONS_HPP

#include <string>

#include <nibbles/gamesettings.hpp>
#include <nibbles/utility/verbosity.hpp>

namespace nibbles { namespace server {

struct Options
{
  Options(int argc, char const* const* const argv);

  bool help;
  utility::Verbosity verbosity;
  bool useTcp;
  std::string tcpAddress;
  uint16_t tcpPort;
  std::string levelPack;
  LevelId startLevel;
  uint32_t startInterval;

  static std::string usage();
  GameSettings gameSettings() const;
};

}}

#endif // NIBBLES_SERVER__OPTIONS_HPP
