#include "remoteplayer.hpp"

namespace nibbles { namespace server {

void RemotePlayer::queueTurn(Direction const dir) const
{
  if (dir == nominalDirection_ ||
    dir == directionOpposite(nominalDirection_)) {
    return;
  }
  nominalDirection_ = dir;
  turnQueue_.push_back(dir);
}

boost::optional<Direction> RemotePlayer::dequeue() const
{
  if (turnQueue_.empty()) return {};
  auto next = turnQueue_.front();
  turnQueue_.pop_front();
  return {next};
}

void RemotePlayer::reset() const
{
  turnQueue_.clear();
  nominalDirection_ = Direction::max;
}

}}

