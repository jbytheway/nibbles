#ifndef NIBBLES__LEVELDEFINITION_HPP
#define NIBBLES__LEVELDEFINITION_HPP

#include <vector>

#include <nibbles/block.hpp>
#include <nibbles/levelid.hpp>
#include <nibbles/position.hpp>

namespace nibbles {

struct LevelDefinition :
  utility::DataClass<
    LevelDefinition,
    LevelId, id,
    std::string, name,
    uint32_t, w,
    uint32_t, h,
    std::vector<Block>, blocks,
    std::vector<std::pair<Block, double>>, randomBlocks,
    std::vector<Position>, starts
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(LevelDefinition)

  LevelDefinition realise() const;
};

}

#endif // NIBBLES__LEVELDEFINITION_HPP

