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

}}}

