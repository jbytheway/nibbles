#ifndef NIBBLES_SERVER__REMOTEPLAYER_HPP
#define NIBBLES_SERVER__REMOTEPLAYER_HPP

#include <nibbles/playerid.hpp>

namespace nibbles { namespace server {

class RemotePlayer {
  public:
    RemotePlayer(
        const Player& player,
        PlayerId id,
        const Connection::Ptr& connection
      ) :
      player_(player, id),
      connection_(connection)
    {}

    const IdedPlayer& player() const { return player_; }
    PlayerId id() const { return player_.get<fields::id>(); }
    Connection::Ptr connection() const { return connection_; }
  private:
    IdedPlayer player_;
    Connection::Ptr connection_;
};

}}

#endif // NIBBLES_SERVER__REMOTEPLAYER_HPP

