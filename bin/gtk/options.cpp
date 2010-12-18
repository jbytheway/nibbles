#include "options.hpp"

#include <cstdlib>
#include <optimal/optionsparser.hpp>
#include <nibbles/network.hpp>

#include "optionserror.hpp"

namespace nibbles { namespace gtk {

Options::Options(int const argc, char** const argv) :
  address("127.0.0.1"),
  connect(false),
  help(false),
  port(Network::defaultPort),
  protocol(client::Protocol::tcp),
  threaded(true),
  verbosity(utility::Verbosity::info),
  playerFile(std::string(getenv("HOME"))+"/.nibbles/players")
{
  std::string optionsFile = std::string(getenv("HOME"))+"/.nibbles/gtk-config";
  optimal::OptionsParser parser;
  parser.addOption("address",     'a', &address);
  parser.addOption("connect",     'c', &connect);
  parser.addOption("font",        'f', &fontPath);
  parser.addOption("glade",       'g', &gladePath);
  parser.addOption("help",        'h', &help);
  parser.addOption("port",        'p', &port);
  parser.addOption("protocol",    'P', &protocol);
  parser.addOption("sounds",      's', &soundPath);
  parser.addOption("threaded",    't', &threaded);
  parser.addOption("verbosity",   'v', &verbosity);
  parser.addOption("player-file", 'y', &playerFile);

  if (parser.parse(optionsFile, argc, argv)) {
    std::ostringstream message;
    message << "error(s) processing options:\n";
    copy(
        parser.getErrors().begin(), parser.getErrors().end(),
        std::ostream_iterator<std::string>(message, "\n")
      );
    throw OptionsError(message.str());
  }
}

void Options::show_help(std::ostream& o) const
{
  auto port = boost::lexical_cast<std::string>(Network::defaultPort);
  o <<
"Usage: nibbles-gtk [OPTIONS...]\n"
"  -a,  --address ADD  Connect to server at ADD (default: 127.0.0.1).\n"
"  -c,  --connect      Automatically try to connect on startup.\n"
"  -f,  --font FILE    Use given ttf font for numbers\n"
"                      (default: use Courier.ttf in directory of binary)\n"
"  -g,  --glade PATH   Find UI .glade files at PATH\n"
"                      (default: directory of exe).\n"
"  -h,  --help         Display this message.\n"
"  -p,  --port PORT    Connect to server at port PORT (default: "+port+").\n"
"  -P,  --protocol PR  Connect using protocol PR (default: TCP).\n"
"  -s,  --sounds PATH   Directory in which sounds can be found\n"
"                      (default: directory of exe).\n"
"  -t-, --no-threaded  Do not run multiple threads.  This harms performance\n"
"                      but aids debugging.\n"
"  -v, --verbosity VERB  Set message verbosity to VERB (default: info).\n"
"Options also read from ~/.nibbles/gtk-congif\n";
}

}}

