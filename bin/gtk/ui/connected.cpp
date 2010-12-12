#include "connected.hpp"

namespace nibbles { namespace gtk { namespace ui {

Connected::Connected(my_context ctx) : my_base(ctx)
{
  post_event(events::Connected());
}

Connected::~Connected()
{
  auto& client = context<Connectedness>().client_;
  context<Machine>().messageHandler().message(
    utility::Verbosity::error, "disconnecting"
  );
  assert(client);

  client->close();
  client.reset();
}

}}}

