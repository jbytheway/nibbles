#ifndef NIBBLES_GTK__UI_HPP
#define NIBBLES_GTK__UI_HPP

#include <boost/utility.hpp>

#include <gtkmm.h>
#include <libglademm.h>

#include "controlledplayer.hpp"
#include <nibbles/client/client.hpp>

#include "options.hpp"

namespace nibbles { namespace gtk {

class UI : public utility::MessageHandler, private boost::noncopyable {
  public:
    UI(
        boost::asio::io_service& io,
        const Options&,
        const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml
      );
    ~UI();
    Gtk::Window& window() { return *window_; }
    void message(utility::Verbosity, const std::string& message);
  private:
    boost::asio::io_service& io_;
    Options options_;

    // controls
    Gtk::Window* window_;
    Glib::RefPtr<Gtk::TextBuffer> messages_;
    
    class PlayerComboColumns : public Gtk::TreeModel::ColumnRecord
    {
      public:
        PlayerComboColumns()
        {
          add(name_);
        }

        Gtk::TreeModelColumn<Glib::ustring> name_;
    };
    Gtk::ComboBox* playerCombo_;
    const PlayerComboColumns playerComboColumns_;
    Glib::RefPtr<Gtk::ListStore> playerComboListStore_;

    Gtk::Entry* playerName_;
    Gtk::ColorButton* playerColor_;
    std::array<Gtk::Button*, Direction::max> playerControlButtons_;

    Gtk::Dialog* newKeyDialog_;
    Gtk::Button* newKeyCancelButton_;

    // game data
    std::vector<ControlledPlayer> localPlayers_;

    client::Client::Ptr client_;

    // Convinience functions
    ControlledPlayer* getCurrentPlayer();

    // File access stuff
    void loadLocalPlayers();
    void saveLocalPlayers();

    // UI update
    void refreshPlayers();
    void refreshPlayer();

    // UI bindings
    void connect();
    bool nameInUse(const std::string& name);
    void createPlayer();
    void deletePlayer();
    void addPlayerToGame();
    void removePlayerFromGame();
    void playerNameChanged();
    void cancelNewKey();
    // Though templated, these functions can be in the .cpp file because
    // they're private
    template<int Direction>
    void setBinding();
    template<int Direction>
    bool newKey(GdkEventKey* event);
};

}}

#endif // NIBBLES_GTK__UI_HPP

