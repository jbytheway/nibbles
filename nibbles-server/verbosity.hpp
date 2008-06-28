#ifndef NIBBLES_SERVER__VERBOSITY_HPP
#define NIBBLES_SERVER__VERBOSITY_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles { namespace server {

#define NIBBLES_SERVER_VERBOSITY_VALUES() (debug)(info)(warning)(error)
NIBBLES_UTILITY_ENUM_HEADER(Verbosity, NIBBLES_SERVER_VERBOSITY_VALUES())
  
}}

#endif // NIBBLES_SERVER__VERBOSITY_HPP

