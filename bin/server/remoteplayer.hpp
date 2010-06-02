#ifndef NIBBLES_SERVER__REMOTEPLAYER_HPP
#define NIBBLES_SERVER__REMOTEPLAYER_HPP

#include <nibbles/playerid.hpp>

namespace nibbles { namespace server {

class RemotePlayer : public IdedPlayer {
  public:
    RemotePlayer(
        const Player& player,
        PlayerId id,
        Connection* connection
      ) :
      IdedPlayer(player, id, connection->id()),
      connection_(connection)
    {}

    PlayerId id() const { return get<fields::id>(); }
    Connection* connection() const { return connection_; }
    ClientId clientId() const { return connection_->id(); }
  private:
    Connection* connection_;
};

}}

#endif // NIBBLES_SERVER__REMOTEPLAYER_HPP

