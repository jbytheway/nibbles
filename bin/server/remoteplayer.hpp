#ifndef NIBBLES_SERVER__REMOTEPLAYER_HPP
#define NIBBLES_SERVER__REMOTEPLAYER_HPP

#include <queue>

#include <nibbles/idedplayer.hpp>

#include "connection.hpp"

namespace nibbles { namespace server {

class RemotePlayer : public IdedPlayer {
  public:
    RemotePlayer(
        const Player& player,
        PlayerId id,
        Connection* connection
      ) :
      IdedPlayer(player, id, connection->id()),
      connection_(connection),
      nominalDirection_(Direction::max)
    {}

    PlayerId id() const { return get<fields::id>(); }
    Connection* connection() const { return connection_; }
    ClientId clientId() const { return connection_->id(); }
    void queueTurn(Direction const) const;
    boost::optional<Direction> dequeue() const;
  private:
    Connection* connection_;
    // HACK: can we do without this mutable??
    mutable Direction nominalDirection_;
    mutable std::queue<Direction> turnQueue_;
};

}}

#endif // NIBBLES_SERVER__REMOTEPLAYER_HPP

