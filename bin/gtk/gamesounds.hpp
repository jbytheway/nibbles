#ifndef NIBBLES_GTK__GAMESOUNDS_HPP
#define NIBBLES_GTK__GAMESOUNDS_HPP

#include "soundservice.hpp"

namespace nibbles { namespace gtk {

struct GameSounds {
  GameSounds(
    SoundService const& service,
    boost::filesystem::path const& soundDir
  ) :
    intro(service.makeSound(soundDir/"intro.flac")),
    start(service.makeSound(soundDir/"start.flac")),
    eat(service.makeSound(soundDir/"eat.flac")),
    death(service.makeSound(soundDir/"death.flac"))
  {}

  typedef std::unique_ptr<Sound> SoundPtr;
  SoundPtr intro;
  SoundPtr start;
  SoundPtr eat;
  SoundPtr death;
};

}}

#endif // NIBBLES_GTK__GAMESOUNDS_HPP

