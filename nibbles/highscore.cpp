#include <nibbles/highscore.hpp>

namespace nibbles {

bool operator<(HighScore const& l, HighScore const& r)
{
  auto const ls = l.get<totalScore>();
  auto const rs = r.get<totalScore>();
  auto const ld = l.get<time>();
  auto const rd = r.get<time>();
  return ls < rs || (ls == rs && ld > rd);
}

}

