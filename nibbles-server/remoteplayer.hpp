#ifndef NIBBLES_SERVER__REMOTEPLAYER_HPP
#define NIBBLES_SERVER__REMOTEPLAYER_HPP

#include <nibbles/playerid.hpp>

namespace nibbles { namespace server {

class RemotePlayer {
  public:
    RemotePlayer(
        const Player& player,
        PlayerId id,
        Connection* connection
      ) :
      player_(player, id, connection->id()),
      connection_(connection)
    {}

    const IdedPlayer& player() const { return player_; }
    PlayerId id() const { return player_.get<fields::id>(); }
    Connection* connection() const { return connection_; }
    ClientId clientId() const { return connection_->id(); }
  private:
    IdedPlayer player_;
    Connection* connection_;
};

}}

#endif // NIBBLES_SERVER__REMOTEPLAYER_HPP

