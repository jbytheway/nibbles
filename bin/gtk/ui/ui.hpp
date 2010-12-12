#ifndef NIBBLES_GTK__UI__UI_HPP
#define NIBBLES_GTK__UI__UI_HPP

#include <boost/utility.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/scoped_ptr.hpp>

#include <libglademm.h>

#include <nibbles/utility/messagehandler.hpp>

#include "options.hpp"
#include "gamesounds.hpp"

namespace nibbles { namespace gtk { namespace ui {

class UI : private boost::noncopyable {
  public:
    UI(
        boost::asio::io_service& io,
        Options const&,
        Glib::RefPtr<Gnome::Glade::Xml> const& gladeXml,
        boost::filesystem::path const& fontPath,
        GameSounds const&
      );
    ~UI();
    bool ended();
  private:
    class Impl;
    boost::scoped_ptr<Impl> impl_;
};

}}}

#endif // NIBBLES_GTK__UI__UI_HPP

