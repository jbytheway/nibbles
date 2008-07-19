#include "options.hpp"

#include <optionsparser.h>
#include <nibbles/network.hpp>

#include "optionserror.hpp"

using namespace std;
using namespace optimal;
using namespace nibbles::utility;

namespace nibbles { namespace server {

Options::Options(int argc, char const* const* const argv) :
  verbosity(Verbosity::debug),
  useTcp(true),
  tcpAddress("127.0.0.1"),
  tcpPort(Network::defaultPort),
  levelPack()
{
  string optionsFile = string(getenv("HOME"))+"/.nibbles/server-config";
  OptionsParser parser;
  parser.addOption("verbosity",   'v', &verbosity);
  parser.addOption("tcp",         't', &useTcp);
  parser.addOption("tcp-addr",    'a', &tcpAddress);
  parser.addOption("tcp-port",    'p', &tcpPort);
  parser.addOption("levels",      'l', &levelPack);

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

