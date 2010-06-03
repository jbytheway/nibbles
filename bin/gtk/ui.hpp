#ifndef NIBBLES_GTK__UI_HPP
#define NIBBLES_GTK__UI_HPP

#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <gtkmm.h>
#include <libglademm.h>

#include "controlledplayer.hpp"
#include <nibbles/client/client.hpp>

#include "options.hpp"
#include "crossthreadsignal.hpp"
#include "remoteplayer.hpp"

namespace nibbles { namespace gtk {

class UI : public utility::MessageHandler, private boost::noncopyable {
  public:
    UI(
        boost::asio::io_service& io,
        const Options&,
        const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& playXml
      );
    ~UI();
    Gtk::Window& window() { return *window_; }
    virtual void message(utility::Verbosity, const std::string& message);
    void disconnect();
  private:
    boost::asio::io_service& io_;
    Options options_;

    CrossThreadSignal<std::string> messageSignal_;
    void writeMessage(const std::string&); // Call only on GUI thread

    CrossThreadSignal<MessageBase::Ptr> netMessageSignal_;
    void handleNetMessage(const MessageBase::Ptr&); // Call only on GUI thread
    template<int Type>
    void internalNetMessage(const Message<Type>&);

    // controls
    Gtk::Window* window_;
    Gtk::TextView* messageView_;
    Gtk::CheckButton* readyCheck_;

    class RemotePlayerListColumns : public Gtk::TreeModel::ColumnRecord
    {
      public:
        RemotePlayerListColumns()
        {
          add(ready_);
          add(id_);
          add(clientId_);
          add(color_);
          add(name_);
        }

        Gtk::TreeModelColumn<bool> ready_;
        Gtk::TreeModelColumn<PlayerId::internal_type> id_;
        Gtk::TreeModelColumn<ClientId::internal_type> clientId_;
        Gtk::TreeModelColumn<Gdk::Color> color_;
        Gtk::TreeModelColumn<Glib::ustring> name_;
    };
    Gtk::TreeView* remotePlayerList_;
    const RemotePlayerListColumns remotePlayerListColumns_;
    Glib::RefPtr<Gtk::ListStore> remotePlayerListStore_;

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

    Gtk::Window* playWindow_;
    Gtk::DrawingArea* levelDisplay_;

    // game data
    class SequenceTag;
    typedef boost::multi_index_container<
        RemotePlayer,
        boost::multi_index::indexed_by<
          boost::multi_index::ordered_unique<
            BOOST_MULTI_INDEX_CONST_MEM_FUN(
                IdedPlayer::base, const PlayerId&, get<id>
              )
          >,
          boost::multi_index::sequenced<
            boost::multi_index::tag<SequenceTag>
          >
        >
      > RemotePlayerContainer;
    RemotePlayerContainer remotePlayers_;
    std::vector<ControlledPlayer> localPlayers_;

    client::Client::Ptr client_;

    // Convinience functions
    ControlledPlayer* getCurrentPlayer();

    // File access stuff
    void loadLocalPlayers();
    void saveLocalPlayers();

    // UI update
    void refreshRemotePlayers();
    void refreshLocalPlayers();
    void refreshLocalPlayer();

    // UI bindings
    void connect();
    bool nameInUse(const std::string& name);
    void createPlayer();
    void deletePlayer();
    void addPlayerToGame();
    void removePlayerFromGame();
    void playerNameChanged();
    void colorChanged();
    void cancelNewKey();
    void readinessChange();
    // Though templated, these functions can be in the .cpp file because
    // they're private
    template<int Direction>
    void setBinding();
    template<int Direction>
    bool newKey(GdkEventKey* event);
};

}}

#endif // NIBBLES_GTK__UI_HPP

