#ifndef NIBBLES__UTILITY__VERBOSITY_HPP
#define NIBBLES__UTILITY__VERBOSITY_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles { namespace utility {

#define NIBBLES_UTILITY_VERBOSITY_VALUES() (debug)(info)(warning)(error)
NIBBLES_UTILITY_ENUM_HEADER(Verbosity, NIBBLES_UTILITY_VERBOSITY_VALUES())

}}

#endif // NIBBLES__UTILITY__VERBOSITY_HPP

