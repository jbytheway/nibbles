#ifndef NIBBLES_GTK__SOUNDSERVICE_HPP
#define NIBBLES_GTK__SOUNDSERVICE_HPP

#include <memory>

#include <boost/filesystem/path.hpp>

#include "sound.hpp"

namespace nibbles { namespace gtk {

class SoundService {
  public:
    SoundService();
    SoundService(SoundService const&) = delete;
    SoundService& operator=(SoundService const&) = delete;

    std::unique_ptr<Sound> makeSound(boost::filesystem::path const&) const;
  private:
    friend class PaSound;

    class Impl;
    std::shared_ptr<Impl> impl_;
};

}}

#endif // NIBBLES_GTK__SOUNDSERVICE_HPP

