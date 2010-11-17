#ifndef NIBBLES_GTK__SOUND_HPP
#define NIBBLES_GTK__SOUND_HPP

namespace nibbles { namespace gtk {

class Sound {
  public:
    Sound(Sound const&) = delete;
    Sound& operator=(Sound const&) = delete;
    virtual ~Sound() = 0;

    virtual void async_play() = 0;
  protected:
    Sound() = default;
};

inline Sound::~Sound() = default;

}}

#endif // NIBBLES_GTK__SOUND_HPP

