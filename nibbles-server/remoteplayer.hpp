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
      player_(player),
      id_(id),
      connection_(connection)
    {}

    PlayerId id() const { return id_; }
    Connection::Ptr connection() const { return connection_; }
  private:
    Player player_;
    PlayerId id_;
    Connection::Ptr connection_;
};

}}

#endif // NIBBLES_SERVER__REMOTEPLAYER_HPP

