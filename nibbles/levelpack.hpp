#ifndef NIBBLES__LEVELPACK_HPP
#define NIBBLES__LEVELPACK_HPP

#include <nibbles/leveldefinition.hpp>

namespace nibbles {

struct LevelPack :
  utility::DataClass<
    LevelPack,
    std::string, name,
    std::vector<LevelDefinition>, levels
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(LevelPack)
};

}

#endif // NIBBLES__LEVELPACK_HPP

