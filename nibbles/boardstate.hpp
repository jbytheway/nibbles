#ifndef NIBBLES__BOARDSTATE_HPP
#define NIBBLES__BOARDSTATE_HPP

#include <nibbles/utility/enum.hpp>

namespace nibbles {

#define NIBBLES_BOARDSTATE_VALUES() (empty)(wall)(snake)(number)
NIBBLES_UTILITY_ENUM_HEADER(BoardState, NIBBLES_BOARDSTATE_VALUES())

}

#endif // NIBBLES__BOARDSTATE_HPP

