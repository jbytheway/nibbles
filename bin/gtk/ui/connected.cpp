#include "connected.hpp"

namespace nibbles { namespace gtk { namespace ui {

Connected::Connected(my_context context) : my_base(context)
{
  client_ = this->context<Machine>().clientFactory().makeClient();
}

Connected::~Connected()
{
  context<Machine>().messageHandler().message(
    utility::Verbosity::error, "disconnecting\n"
  );
  assert(client_);

  client_->close();
  client_.reset();
}

}}}

