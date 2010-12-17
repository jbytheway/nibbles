#ifndef NIBBLES_SERVER__OPTIONS_HPP
#define NIBBLES_SERVER__OPTIONS_HPP

#include <string>

#include <boost/filesystem/path.hpp>

#include <nibbles/gamesettings.hpp>
#include <nibbles/utility/verbosity.hpp>

namespace nibbles { namespace server {

struct Options
{
  Options(int argc, char const* const* const argv);

  std::string tcpAddress;
  LevelId startLevel;
  uint32_t countdown;
  double intervalFactor;
  bool help;
  uint32_t startInterval;
  std::string levelPack;
  uint16_t tcpPort;
  boost::filesystem::path highScores;
  bool useTcp;
  utility::Verbosity verbosity;

  static std::string usage();
  GameSettings gameSettings() const;
};

}}

#endif // NIBBLES_SERVER__OPTIONS_HPP

