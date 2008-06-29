#ifndef NIBBLES_GTK__UI_HPP
#define NIBBLES_GTK__UI_HPP

#include <boost/utility.hpp>

#include <gtkmm.h>
#include <libglademm.h>

#include <nibbles/client/client.hpp>

#include "options.hpp"

namespace nibbles { namespace gtk {

class UI : public utility::MessageHandler, private boost::noncopyable {
  public:
    UI(
        boost::asio::io_service& io,
        const Options&,
        const Glib::RefPtr<Gnome::Glade::Xml>& refXml
      );
    Gtk::Window& window() { return *window_; }
    void message(utility::Verbosity, const std::string& message);
  private:
    boost::asio::io_service& io_;
    Options options_;
    Gtk::Window* window_;
    Glib::RefPtr<Gtk::TextBuffer> messages_;
    client::Client::Ptr client_;

    void connect();
    void createPlayer();
    void deletePlayer();
    void addPlayerToGame();
    void removePlayerFromGame();
    // Though it's templated, this function can be in the .cpp file because
    // it's private
    template<int Direction>
    void setBinding();
};

}}

#endif // NIBBLES_GTK__UI_HPP

