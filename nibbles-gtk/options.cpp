#include "options.hpp"

#include <cstdlib>
#include <optionsparser.h>
#include <nibbles/network.hpp>

#include "optionserror.hpp"

using namespace std;
using namespace optimal;
using namespace nibbles::utility;
using namespace nibbles::client;

namespace nibbles { namespace gtk {

Options::Options(int const argc, char** const argv) :
  verbosity(Verbosity::info),
  threaded(true),
  playerFile(string(getenv("HOME"))+"/.nibbles/players"),
  protocol(Protocol::tcp),
  address("127.0.0.1"),
  port(Network::defaultPort)
{
  string optionsFile = string(getenv("HOME"))+"/.nibbles/gtk-config";
  OptionsParser parser;
  parser.addOption("verbosity",   'v', &verbosity);
  parser.addOption("threaded",    't', &threaded);
  parser.addOption("player-file", 'y', &playerFile);
  parser.addOption("protocol",    'P', &protocol);
  parser.addOption("address",     'a', &address);
  parser.addOption("port",        'p', &port);

  if (parser.parse(optionsFile, argc, argv)) {
    ostringstream message;
    message << "error(s) processing options:\n";
    copy(
        parser.getErrors().begin(), parser.getErrors().end(),
        ostream_iterator<string>(message, "\n")
      );
    throw OptionsError(message.str());
  }
}

}}

