#ifndef NIBBLES_CLIENT__PROTOCOL_HPP
#define NIBBLES_CLIENT__PROTOCOL_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles { namespace client {

#define NIBBLES_CLIENT_PROTOCOL_VALUES() (udp)(tcp)

NIBBLES_UTILITY_ENUM_HEADER(Protocol, NIBBLES_CLIENT_PROTOCOL_VALUES())

}}

#endif // NIBBLES_CLIENT__PROTOCOL_HPP

