#include <nibbles/scoretracker.hpp>

namespace nibbles {

void ScoreTracker::add(PlayerId const player, LifeCount const lives)
{
  assert(!scores_.count(player));
  scores_.insert({player, 0});
  lives_.insert({player, lives});
}

void ScoreTracker::clear()
{
  scores_.clear();
  lives_.clear();
}

Score ScoreTracker::getScore(PlayerId const player) const
{
  auto it = scores_.find(player);
  assert(it != scores_.end());
  return it->second;
}

LifeCount ScoreTracker::getLives(PlayerId const player) const
{
  auto it = lives_.find(player);
  assert(it != lives_.end());
  return it->second;
}

void ScoreTracker::addScore(PlayerId const player, Score const score)
{
  auto it = scores_.find(player);
  assert(it != scores_.end());
  it->second += score;
}

bool ScoreTracker::deductLife(PlayerId const player)
{
  auto it = lives_.find(player);
  assert(it != lives_.end());
  assert(it->second > 0);
  return 0 == --it->second;
}

}

