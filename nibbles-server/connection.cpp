#include "connection.hpp"

#include <boost/bind.hpp>

namespace nibbles { namespace server {

Connection::Connection(const Socket::Ptr& socket) :
  socket_(socket)
{
  socket_->messageSignal.connect(
      boost::bind(boost::ref(messageSignal), _1, this)
    );
  socket_->terminateSignal.connect(
      boost::bind(boost::ref(terminateSignal), this)
    );
  // TODO: should disconnect these on destruction
}

}}

