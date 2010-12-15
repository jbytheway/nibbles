#include "options.hpp"

#include <optimal/optionsparser.hpp>
#include <nibbles/network.hpp>

#include "optionserror.hpp"

using namespace std;
using namespace optimal;
using namespace nibbles::utility;

namespace nibbles { namespace server {

Options::Options(int argc, char const* const* const argv) :
  help(false),
  verbosity(Verbosity::debug),
  useTcp(true),
  tcpAddress("127.0.0.1"),
  tcpPort(Network::defaultPort),
  levelPack(),
  highScores(string(getenv("HOME"))+"/.nibbles/server-highscores"),
  startLevel(),
  startInterval(100),
  intervalFactor(0.98),
  countdown(44)
{
  string optionsFile = string(getenv("HOME"))+"/.nibbles/server-config";
  unsigned int startLevel;
  OptionsParser parser;
  parser.addOption("help",        'h', &help);
  parser.addOption("verbosity",   'v', &verbosity);
  parser.addOption("tcp",         't', &useTcp);
  parser.addOption("tcp-addr",    'a', &tcpAddress);
  parser.addOption("tcp-port",    'p', &tcpPort);
  parser.addOption("levels",      'l', &levelPack);
  parser.addOption("high-scores", 's', &highScores);
  parser.addOption("level",       'b', &startLevel);
  parser.addOption("interval",    'i', &startInterval);
  parser.addOption("factor",      'f', &intervalFactor);
  parser.addOption("countdown",   'c', &countdown);

  if (parser.parse(optionsFile, argc, argv)) {
    ostringstream message;
    message << "error(s) processing options:\n";
    copy(
        parser.getErrors().begin(), parser.getErrors().end(),
        ostream_iterator<string>(message, "\n")
      );
    throw OptionsError(message.str());
  }

  if (help) {
    throw OptionsError(usage());
  }
}

string Options::usage()
{
  ostringstream result;
  result <<
"Usage: nibbles-server [OPTIONS...]\n"
"  -h, --help              Display this message\n"
"  -v, --verbosity VERB    Set verbosity to error, warning, info or debug\n"
"  -t, --tcp               Serve on TCP (default on)\n"
"  -a, --tcp-addr ADDR     Address for TCP server (default 127.0.0.1)\n"
"  -p, --tcp-port PORT     Port for TCP server (default " <<
  Network::defaultPort << ")\n"
"  -l, --levels FILE       Specify level pack (no default)\n"
"  -b, --level LEVEL       Specify start level (default 0)\n"
"  -s, --high-scores FILE  Specify file to store high scores\n"
"                          (default ~/.nibbles/server-highscores)\n"
"  -i, --interval INT      Specify initial tick interval in milliseconds\n"
"                          (default 100)\n"
"  -f, --factor FACT       Specify factor tick interval multiplied by\n"
"                          (default 0.98)\n"
"  -c, --countdown NUM     How many ticks for which to count down to level\n"
"                          start (default 44)\n"
"Options also read from ~/.nibbles/server-config\n";
  return result.str();
}

GameSettings Options::gameSettings() const
{
  return GameSettings(
      startLevel,
      boost::posix_time::milliseconds(startInterval),
      intervalFactor /* Interval factor */,
      5 /* Start lives */,
      2 /* Start length */,
      3 /* Growth rate */,
      -10 /* Death score */
    );
}

}}

