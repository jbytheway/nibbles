#ifndef NIBBLES__LEVELDEFINITION_HPP
#define NIBBLES__LEVELDEFINITION_HPP

#include <nibbles/block.hpp>
#include <nibbles/levelid.hpp>

namespace nibbles {

struct LevelDefinition :
  utility::DataClass<
    LevelDefinition,
    LevelId, id,
    uint32_t, w,
    uint32_t, h,
    std::vector<Block>, blocks
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(LevelDefinition)
};

}

#endif // NIBBLES__LEVELDEFINITION_HPP

