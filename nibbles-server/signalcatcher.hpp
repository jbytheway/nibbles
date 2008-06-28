#ifndef NIBBLES_SERVER__SIGNALCATCHER_HPP
#define NIBBLES_SERVER__SIGNALCATCHER_HPP

#include <boost/signal.hpp>

namespace nibbles { namespace server {

extern boost::signal<void (int)> signalCatcher;

}}

#endif // NIBBLES_SERVER__SIGNALCATCHER_HPP

