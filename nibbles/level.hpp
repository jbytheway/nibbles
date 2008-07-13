#ifndef NIBBLES__LEVEL_HPP
#define NIBBLES__LEVEL_HPP

#include <nibbles/leveldefinition.hpp>
#include <nibbles/snake.hpp>
#include <nibbles/number.hpp>

namespace nibbles {

struct Level :
  utility::DataClass<
    Level,
    LevelDefinition, definition,
    std::vector<Snake>, snakes,
    Number, number
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Level)
};

}

#endif // NIBBLES__LEVEL_HPP

