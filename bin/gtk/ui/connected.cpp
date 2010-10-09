#include "connected.hpp"

namespace nibbles { namespace gtk { namespace ui {

Connected::~Connected()
{
  auto& client = context<Connectedness>().client_;
  context<Machine>().messageHandler().message(
    utility::Verbosity::error, "disconnecting\n"
  );
  assert(client);

  client->close();
  client.reset();
}

}}}

