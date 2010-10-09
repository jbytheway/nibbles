#include "notconnected.hpp"

#include <boost/system/system_error.hpp>

namespace nibbles { namespace gtk { namespace ui {

sc::result NotConnected::react(events::Connect const&)
{
  auto& client = context<Connectedness>().client_;
  try {
    client = this->context<Machine>().clientFactory().makeClient();
    assert(client);
    return transit<Connected>();
  } catch (boost::system::system_error const&) {
    return discard_event();
  }
}

}}}

