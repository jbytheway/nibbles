#include "options.hpp"

#include <optimal/optionsparser.hpp>
#include <nibbles/network.hpp>

#include "optionserror.hpp"

namespace nibbles { namespace server {

Options::Options(int argc, char const* const* const argv) :
  tcpAddress("127.0.0.1"),
  startLevel(),
  countdown(44),
  deathScore(-10),
  intervalFactor(0.98),
  growthRate(3),
  help(false),
  startInterval(100),
  levelPack(),
  startLength(2),
  tcpPort(Network::defaultPort),
  highScores(std::string(getenv("HOME"))+"/.nibbles/server-highscores"),
  useTcp(true),
  verbosity(utility::Verbosity::debug),
  startLives(5)
{
  std::string optionsFile =
    std::string(getenv("HOME"))+"/.nibbles/server-config";
  unsigned int startLevel;
  optimal::OptionsParser parser;
  parser.addOption("tcp-addr",    'a', &tcpAddress);
  parser.addOption("level",       'b', &startLevel);
  parser.addOption("countdown",   'c', &countdown);
  parser.addOption("death",       'd', &deathScore);
  parser.addOption("factor",      'f', &intervalFactor);
  parser.addOption("growth",      'g', &growthRate);
  parser.addOption("help",        'h', &help);
  parser.addOption("interval",    'i', &startInterval);
  parser.addOption("levels",      'l', &levelPack);
  parser.addOption("length",      'L', &startLength);
  parser.addOption("tcp-port",    'p', &tcpPort);
  parser.addOption("high-scores", 's', &highScores);
  parser.addOption("tcp",         't', &useTcp);
  parser.addOption("verbosity",   'v', &verbosity);
  parser.addOption("lives",       'V', &startLives);

  if (parser.parse(optionsFile, argc, argv)) {
    std::ostringstream message;
    message << "error(s) processing options:\n";
    copy(
        parser.getErrors().begin(), parser.getErrors().end(),
        std::ostream_iterator<std::string>(message, "\n")
      );
    throw OptionsError(message.str());
  }

  if (help) {
    throw OptionsError(usage());
  }
}

std::string Options::usage()
{
  std::ostringstream result;
  result <<
"Usage: nibbles-server [OPTIONS...]\n"
"  -a, --tcp-addr ADDR     Address for TCP server (default 127.0.0.1)\n"
"  -b, --level LEVEL       Specify start level (default 0)\n"
"  -c, --countdown NUM     How many ticks for which to count down to level\n"
"                          start (default 44)\n"
"  -d, --death SCORE       Score for dying (default -10)\n"
"  -f, --factor FACT       Specify factor tick interval multiplied by\n"
"                          (default 0.98)\n"
"  -g, --growth LENGTH     Growth of snake per point eaten (default 3)\n"
"  -h, --help              Display this message\n"
"  -i, --interval INT      Specify initial tick interval in milliseconds\n"
"                          (default 100)\n"
"  -l, --levels FILE       Specify level pack (no default)\n"
"  -L, --length LENGTH     Initial length of snake (default 2)\n"
"  -p, --tcp-port PORT     Port for TCP server (default " <<
  Network::defaultPort << ")\n"
"  -s, --high-scores FILE  Specify file to store high scores\n"
"                          (default ~/.nibbles/server-highscores)\n"
"  -t, --tcp               Serve on TCP (default on)\n"
"  -v, --verbosity VERB    Set verbosity to error, warning, info or debug\n"
"  -V, --lives             Initial lives (default 5)\n"
"Options also read from ~/.nibbles/server-config\n";
  return result.str();
}

GameSettings Options::gameSettings() const
{
  return GameSettings(
      startLevel,
      boost::posix_time::milliseconds(startInterval),
      intervalFactor,
      startLives,
      startLength,
      growthRate,
      deathScore
    );
}

}}

