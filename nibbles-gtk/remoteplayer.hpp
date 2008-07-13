#ifndef NIBBLES_GTK__REMOTEPLAYER_HPP
#define NIBBLES_GTK__REMOTEPLAYER_HPP

#include <nibbles/idedplayer.hpp>

namespace nibbles { namespace gtk {

struct RemotePlayer :
  utility::DataClass<
    RemotePlayer,
    utility::InheritFrom<IdedPlayer>,
    bool, ready
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(RemotePlayer)
};

}}

#endif // NIBBLES_GTK__REMOTEPLAYER_HPP

