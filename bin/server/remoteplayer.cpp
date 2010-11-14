#include "remoteplayer.hpp"

namespace nibbles { namespace server {

void RemotePlayer::queueTurn(Direction const dir) const
{
  if (dir == nominalDirection_ ||
    dir == directionOpposite(nominalDirection_)) {
    return;
  }
  nominalDirection_ = dir;
  turnQueue_.push(dir);
}

boost::optional<Direction> RemotePlayer::dequeue() const
{
  if (turnQueue_.empty()) return {};
  auto next = turnQueue_.front();
  turnQueue_.pop();
  return {next};
}

}}

