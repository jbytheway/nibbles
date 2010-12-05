#include <nibbles/scoretracker.hpp>

namespace nibbles {

void ScoreTracker::add(PlayerId const player)
{
  assert(!scores_.count(player));
  scores_.insert({player, 0});
}

Score ScoreTracker::operator[](PlayerId const player) const
{
  auto it = scores_.find(player);
  assert(it != scores_.end());
  return it->second;
}

void ScoreTracker::operator()(PlayerId const player, Score const score)
{
  auto it = scores_.find(player);
  assert(it != scores_.end());
  it->second += score;
}

}

