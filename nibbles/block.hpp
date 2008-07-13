#ifndef NIBBLES__BLOCK_HPP
#define NIBBLES__BLOCK_HPP

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>

namespace nibbles {

struct Block :
  utility::DataClass<
    Block,
    uint32_t, x,
    uint32_t, y,
    uint32_t, w,
    uint32_t, h
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Block)
};

}

#endif // NIBBLES__BLOCK_HPP

