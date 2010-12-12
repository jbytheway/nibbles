#include "machine.hpp"

namespace nibbles { namespace gtk { namespace ui {

Machine::Machine(
  utility::MessageHandler& messageHandler,
  ClientFactory& clientFactory,
  boost::filesystem::path playerFile,
  Glib::RefPtr<Gnome::Glade::Xml> const& gladeXml,
  boost::filesystem::path const& fontPath,
  GameSounds const& sounds
) :
  messageHandler_(messageHandler),
  clientFactory_(clientFactory),
  playerFile_(playerFile),
  gladeXml_(gladeXml),
  fontPath_(fontPath),
  sounds_(sounds)
{}

bool Machine::ended()
{
  return state_cast<Terminated const*>();
}

void Machine::dump()
{
  std::string result = "State machine dump:\n";
  for (auto leaf = state_begin(); leaf != state_end(); ++leaf) {
    result += typeid(*leaf).name();
    result += "\n";
  }
  messageHandler_.message(utility::Verbosity::debug, result);
}

}}}

