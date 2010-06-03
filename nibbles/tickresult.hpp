#ifndef NIBBLES__TICKRESULT_HPP
#define NIBBLES__TICKRESULT_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles {

#define NIBBLES_TICKRESULT_VALUES() (none)(number)(dead)
NIBBLES_UTILITY_ENUM_HEADER(TickResult, NIBBLES_TICKRESULT_VALUES())

}

#endif // NIBBLES__TICKRESULT_HPP

