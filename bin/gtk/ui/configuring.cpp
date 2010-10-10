#include "configuring.hpp"

#include <array>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <gtkmm.h>

#include <nibbles/playerid.hpp>
#include <nibbles/clientid.hpp>
#include <nibbles/direction.hpp>
#include <nibbles/message.hpp>
#include <nibbles/utility/verbosity.hpp>
#include <nibbles/utility/messagehandler.hpp>

#include "remoteplayer.hpp"
#include "controlledplayer.hpp"
#include "colorconverter.hpp"

namespace nibbles { namespace gtk { namespace ui {

class Configuring::Impl {
  public:
    Impl(
        Configuring* parent,
        const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml
      );
    ~Impl();

    // Implementation of MessageSink interface
    virtual void message(std::string const&);

    // Network reactions
    void playerAdded(const Message<MessageType::playerAdded>&);
    void updateReadiness(const Message<MessageType::updateReadiness>&);
    void connected();
    void disconnected();
  private:
    // Link back to state machine
    Configuring* parent_;

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
    std::map<ClientId, bool> clientReadiness_;

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
    void windowClosed();
    bool isNameInUse(const std::string& name);
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

Configuring::Configuring(my_context context) :
  my_base(context),
  impl_(new Impl(
      this,
      this->context<Machine>().mainXml(),
      this->context<Machine>().newKeyXml()
    ))
{}

Configuring::~Configuring() = default;

void Configuring::message(std::string const& message) const
{
  impl_->message(message);
}

sc::result Configuring::react(
  events::Message<MessageType::playerAdded> const& event
)
{
  impl_->playerAdded(event.message);
  return discard_event();
}

sc::result Configuring::react(
  events::Message<MessageType::updateReadiness> const& event
)
{
  impl_->updateReadiness(event.message);
  return discard_event();
}

sc::result Configuring::react(events::Disconnect const&)
{
  impl_->disconnected();
  return forward_event();
}

sc::result Configuring::react(events::Connected const&)
{
  impl_->connected();
  return forward_event();
}

Configuring::Impl::Impl(
  Configuring* parent,
  const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
  const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml
) :
  parent_(parent),
  window_(NULL),
  playerCombo_(NULL)
{
#define GET_WIDGET(xml, type, name)              \
  Gtk::type* w##name = NULL;                     \
  do {                                           \
    xml##Xml->get_widget(#name, w##name);        \
    if (!w##name) {                              \
      throw std::runtime_error("missing "#name); \
    }                                            \
  } while (false)

  GET_WIDGET(main, Window, MainWindow);
  GET_WIDGET(main, TextView, MessageText);
  GET_WIDGET(main, Entry, ServerAddressEntry);
  GET_WIDGET(main, CheckButton, ReadyCheck);
  GET_WIDGET(main, TreeView, RemotePlayerList);
  GET_WIDGET(main, ComboBox, PlayerCombo);
  GET_WIDGET(main, Entry, PlayerNameEntry);
  GET_WIDGET(main, ColorButton, PlayerColorButton);
  GET_WIDGET(main, Button, ConnectButton);
  GET_WIDGET(main, Button, CreateButton);
  GET_WIDGET(main, Button, DeleteButton);
  GET_WIDGET(main, Button, AddButton);
  GET_WIDGET(main, Button, RemoveButton);
  GET_WIDGET(main, Button, UpButton);
  GET_WIDGET(main, Button, DownButton);
  GET_WIDGET(main, Button, LeftButton);
  GET_WIDGET(main, Button, RightButton);

  GET_WIDGET(newKey, Dialog, NewKeyDialog);
  GET_WIDGET(newKey, Button, NewKeyCancelButton);
#undef GET_WIDGET

  // Store pointers to those widgets we need to access later
  window_ = wMainWindow;
  messageView_ = wMessageText;
  readyCheck_ = wReadyCheck;
  remotePlayerList_ = wRemotePlayerList;
  playerCombo_ = wPlayerCombo;
  playerName_ = wPlayerNameEntry;
  playerColor_ = wPlayerColorButton;
  playerControlButtons_[Direction::up] = wUpButton;
  playerControlButtons_[Direction::down] = wDownButton;
  playerControlButtons_[Direction::left] = wLeftButton;
  playerControlButtons_[Direction::right] = wRightButton;

  newKeyDialog_ = wNewKeyDialog;
  newKeyCancelButton_ = wNewKeyCancelButton;

  // Attach the columns to their controls
  remotePlayerListStore_ = Gtk::ListStore::create(remotePlayerListColumns_);
  assert(remotePlayerListStore_);
  remotePlayerList_->set_model(remotePlayerListStore_);
  remotePlayerList_->append_column("Ready", remotePlayerListColumns_.ready_);
  remotePlayerList_->append_column("ID", remotePlayerListColumns_.id_);
  remotePlayerList_->append_column("Client", remotePlayerListColumns_.clientId_);
  // TODO: make color display work
  //remotePlayerList_->append_column("Colour", remotePlayerListColumns_.color_);
  remotePlayerList_->append_column("Name", remotePlayerListColumns_.name_);

  playerComboListStore_ = Gtk::ListStore::create(playerComboColumns_);
  assert(playerComboListStore_);
  playerCombo_->set_model(playerComboListStore_);
  playerCombo_->pack_start(playerComboColumns_.name_);

  // Connect signals from widgets to the UI
#define CONNECT_BUTTON(buttonName, memFunName)     \
  w##buttonName##Button->signal_clicked().connect( \
      sigc::mem_fun(this, &Impl::memFunName)         \
    );
  CONNECT_BUTTON(Connect, connect);
  CONNECT_BUTTON(Create, createPlayer);
  CONNECT_BUTTON(Delete, deletePlayer);
  CONNECT_BUTTON(Add, addPlayerToGame);
  CONNECT_BUTTON(Remove, removePlayerFromGame);
  CONNECT_BUTTON(Up, setBinding<Direction::up>);
  CONNECT_BUTTON(Down, setBinding<Direction::down>);
  CONNECT_BUTTON(Left, setBinding<Direction::left>);
  CONNECT_BUTTON(Right, setBinding<Direction::right>);
  CONNECT_BUTTON(NewKeyCancel, cancelNewKey);
#undef CONNECT_BUTTON
  window_->signal_hide().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  );
  wReadyCheck->signal_toggled().connect(
    sigc::mem_fun(this, &Impl::readinessChange)
  );
  wPlayerColorButton->signal_color_set().connect(
    sigc::mem_fun(this, &Impl::colorChanged)
  );
  playerCombo_->signal_changed().connect(
    sigc::mem_fun(this, &Impl::refreshLocalPlayer)
  );
  playerName_->signal_changed().connect(
    sigc::mem_fun(this, &Impl::playerNameChanged)
  );

  loadLocalPlayers();

  // Finally, show the GUI
  window_->show();
}

Configuring::Impl::~Impl()
{
  saveLocalPlayers();
}

void Configuring::Impl::message(const std::string& message)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  buffer->insert(buffer->end(), message);
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

void Configuring::Impl::playerAdded(
  const Message<MessageType::playerAdded>& netMessage
)
{
  RemotePlayer newPlayer(netMessage.payload());
  bool inserted = remotePlayers_.insert(newPlayer).second;
  if (!inserted) {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "duplicate player added\n"
    );
  }
  refreshRemotePlayers();
}

void Configuring::Impl::updateReadiness(
    const Message<MessageType::updateReadiness>& netMessage
  )
{
  ClientId id = netMessage.payload().first;
  bool ready = netMessage.payload().second;
  clientReadiness_[id] = ready;
  refreshRemotePlayers();
}

ControlledPlayer* Configuring::Impl::getCurrentPlayer()
{
  Gtk::TreeModel::iterator iter = playerCombo_->get_active();
  if (!iter)
    return NULL;
  // TODO: Do we need a faster-than-linear implementation?
  size_t index = std::distance(playerComboListStore_->children().begin(), iter);
  assert(index < localPlayers_.size());
  return &localPlayers_[index];
}

void Configuring::Impl::connected()
{
  // If readiness is checked, need to tell server
  if (readyCheck_->get_active()) {
    auto const& client = parent_->state_cast<Connectedness const&>().client();
    client->setReadiness(true);
  }
}

void Configuring::Impl::disconnected()
{
  remotePlayers_.clear();
  refreshRemotePlayers();
}

void Configuring::Impl::loadLocalPlayers()
{
  using namespace boost::filesystem;
  path playerFilePath(parent_->context<Machine>().playerFile());
  if (!exists(playerFilePath)) {
    return;
  }
  ifstream ifs(playerFilePath);
  boost::archive::xml_iarchive ia(ifs);
  ia >> BOOST_SERIALIZATION_NVP(localPlayers_);
  refreshLocalPlayers();
}

void Configuring::Impl::saveLocalPlayers()
{
  using namespace boost::filesystem;
  path playerFilePath(parent_->context<Machine>().playerFile());
  path playerFileDir = playerFilePath.branch_path();
  path tempPlayerFilePath = playerFileDir/(playerFilePath.filename()+".new");
  if (!exists(playerFileDir)) {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::info,
      "creating directory "+playerFileDir.string()+"\n"
    );
    create_directories(playerFileDir);
  }

  {
    boost::filesystem::ofstream ofs(tempPlayerFilePath);
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(localPlayers_);
  }
  if (exists(playerFilePath)) {
    remove(playerFilePath);
  }
  rename(tempPlayerFilePath, playerFilePath);
}

void Configuring::Impl::refreshRemotePlayers()
{
  // Save the current player so we can put things back properly afterwards
  PlayerId currentId = PlayerId::invalid();
  if (Gtk::TreeModel::iterator iter =
      remotePlayerList_->get_selection()->get_selected())
    currentId = PlayerId::fromInteger((*iter)[remotePlayerListColumns_.id_]);

  // Clear out the list box
  remotePlayerListStore_->clear();
  BOOST_FOREACH(const RemotePlayer& player, remotePlayers_) {
    auto const cId = player.get<clientId>();
    auto const readinessIt = clientReadiness_.find(cId);
    bool ready = false;
    if (readinessIt != clientReadiness_.end()) {
      ready = readinessIt->second;
    }
    Gtk::TreeModel::iterator iter = remotePlayerListStore_->append();
    Gtk::TreeModel::Row row = *iter;
    row[remotePlayerListColumns_.ready_] = ready;
    row[remotePlayerListColumns_.id_] = player.get<id>();
    row[remotePlayerListColumns_.clientId_] = cId;
    row[remotePlayerListColumns_.color_] =
      ColorConverter::toGdkColor(player.get<color>());
    row[remotePlayerListColumns_.name_] = player.get<name>();
    if (player.get<id>() == currentId) {
      remotePlayerList_->get_selection()->select(iter);
    }
  }
}

void Configuring::Impl::refreshLocalPlayers()
{
  // Save the current player so we can put things back properly afterwards
  std::string currentName;
  if (ControlledPlayer* currentPlayer = getCurrentPlayer())
    currentName = currentPlayer->get<name>();

  // Clear out the combo box
  playerComboListStore_->clear();
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers_) {
    Gtk::TreeModel::iterator iter = playerComboListStore_->append();
    Gtk::TreeModel::Row row = *iter;
    std::string name = player.get<fields::name>();
    row[playerComboColumns_.name_] = name;
    if (name == currentName) {
      playerCombo_->set_active(iter);
    }
  }

  refreshLocalPlayer();
}

void Configuring::Impl::refreshLocalPlayer()
{
  const ControlledPlayer* player = getCurrentPlayer();
  if (player) {
    playerName_->set_text(player->get<name>());
    playerColor_->set_color(ColorConverter::toGdkColor(player->get<color>()));
    for (int d = 0; d < Direction::max; ++d) {
      playerControlButtons_[d]->set_label(
          Direction(d).string()+": "+gdk_keyval_name(player->get<controls>()[d])
        );
    }
  } else {
    playerName_->set_text("");
    playerColor_->set_color(ColorConverter::toGdkColor(Color::black));
    for (int d = 0; d < Direction::max; ++d) {
      playerControlButtons_[d]->set_label(Direction(d).string());
    }
  }
}

void Configuring::Impl::connect()
{
  // Crazy dangerous risking reentrancy madness
  parent_->context<Machine>().process_event(events::Connect());
}

void Configuring::Impl::windowClosed()
{
  // Crazy dangerous risking reentrancy madness
  parent_->context<Machine>().process_event(events::Terminate());
}

bool Configuring::Impl::isNameInUse(const std::string& name)
{
  // TODO: Do we need a faster-than-linear implementation?
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers_) {
    if (player.get<fields::name>() == name) {
      return true;
    }
  }

  return false;
}

void Configuring::Impl::createPlayer()
{
  std::string newName = "New player";

  if (isNameInUse(newName)) {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "name '"+newName+"' in use\n"
    );
    return;
  }

  std::array<uint32_t, Direction::max> newControls;
  newControls[Direction::up] = GDK_Up;
  newControls[Direction::down] = GDK_Down;
  newControls[Direction::left] = GDK_Left;
  newControls[Direction::right] = GDK_Right;

  ControlledPlayer newPlayer(
      Player(newName, Color::yellow),
      newControls
    );
  localPlayers_.push_back(newPlayer);
  assert(playerComboListStore_);
  Gtk::TreeModel::iterator iter = playerComboListStore_->append();
  (*iter)[playerComboColumns_.name_] = newName;
  playerCombo_->set_active(iter);

  refreshLocalPlayer();
}

void Configuring::Impl::deletePlayer()
{
  NIBBLES_FATAL("not implemented");
}

void Configuring::Impl::addPlayerToGame()
{
  Player* currentPlayer = getCurrentPlayer();
  if (!currentPlayer) {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "no player selected\n"
    );
    return;
  }
  if (auto const& client =
      parent_->state_cast<Connectedness const&>().client()) {
    client->addPlayer(*currentPlayer);
  } else {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "not connected\n"
    );
  }
}

void Configuring::Impl::removePlayerFromGame()
{
  NIBBLES_FATAL("not implemented");
}

void Configuring::Impl::playerNameChanged()
{
  std::string newName = playerName_->get_text();
  if (isNameInUse(newName)) {
    return;
  }

  if (Player* currentPlayer = getCurrentPlayer()) {
    currentPlayer->get<name>() = newName;
    (*playerCombo_->get_active())[playerComboColumns_.name_] = newName;
  }
}

void Configuring::Impl::colorChanged()
{
  if (Player* currentPlayer = getCurrentPlayer()) {
    currentPlayer->get<color>() =
      ColorConverter::toColor(playerColor_->get_color());
  }
}

void Configuring::Impl::cancelNewKey()
{
  newKeyDialog_->hide();
}

void Configuring::Impl::readinessChange()
{
  bool readiness = readyCheck_->get_active();
  if (auto const& client =
      parent_->state_cast<Connectedness const&>().client()) {
    client->setReadiness(readiness);
  }
}

template<int Direction>
void Configuring::Impl::setBinding()
{
  if (!getCurrentPlayer())
    return;
  sigc::connection connection0 =
    newKeyDialog_->signal_key_press_event().connect(
      sigc::mem_fun(this, &Impl::newKey<Direction>)
    );
  sigc::connection connection1 =
    newKeyCancelButton_->signal_key_press_event().connect(
      sigc::mem_fun(this, &Impl::newKey<Direction>)
    );
  newKeyDialog_->run();
  // Bizarrely, if close button clicked, doesn't hide but does return from
  // run(), so we hide manually
  newKeyDialog_->hide();
  parent_->context<Machine>().messageHandler().message(
    utility::Verbosity::debug, "new key dialog closed\n"
  );
  connection0.disconnect();
  connection1.disconnect();
  refreshLocalPlayer();
}

template<int Direction>
bool Configuring::Impl::newKey(GdkEventKey* event)
{
  uint32_t keyval = event->keyval;
  ControlledPlayer* player = getCurrentPlayer();
  if (player)
    player->get<controls>()[Direction] = keyval;
  newKeyDialog_->hide();
  return false;
}

}}}

