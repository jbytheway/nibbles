#include <nibbles/gamesettings.hpp>

#include <nibbles/utility/dataclasscompare.hpp>

namespace nibbles {

bool operator<(GameSettings const& l, GameSettings const& r)
{
  return utility::DataClassCompare<GameSettings::base>()(l, r);
}

}


