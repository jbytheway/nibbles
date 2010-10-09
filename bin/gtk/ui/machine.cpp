#include "machine.hpp"

namespace nibbles { namespace gtk { namespace ui {

Machine::Machine(
  utility::MessageHandler& messageHandler,
  ClientFactory& clientFactory,
  boost::filesystem::path playerFile,
  const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
  const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml
) :
  messageHandler_(messageHandler),
  clientFactory_(clientFactory),
  playerFile_(playerFile),
  mainXml_(mainXml),
  newKeyXml_(newKeyXml)
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

