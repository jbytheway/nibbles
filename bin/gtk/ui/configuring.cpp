#include "configuring.hpp"

#include <array>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <gtkmm.h>

#include <nibbles/playerid.hpp>
#include <nibbles/clientid.hpp>
#include <nibbles/direction.hpp>
#include <nibbles/message.hpp>
#include <nibbles/utility/verbosity.hpp>
#include <nibbles/utility/messagehandler.hpp>

#include "colorconverter.hpp"

namespace nibbles { namespace gtk { namespace ui {

class Configuring::Impl {
  public:
    Impl(
        Configuring* parent,
        const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml
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

    // event handler connections
    std::vector<sigc::connection> uiConnections_;

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
    std::array<Gtk::Button*, Command::max> playerControlButtons_;

    Gtk::Dialog* newKeyDialog_;
    Gtk::Button* newKeyCancelButton_;

    // game data
    std::map<ClientId, bool> clientReadiness_;

    // Convinience functions
    Active::RemotePlayerContainer& remotePlayers();
    Active::LocalPlayerContainer& localPlayers();
    ControlledPlayer const* getCurrentPlayer();
    void addPlayerToGame(Player const&);

    template<typename F>
    void modifyLocalPlayer(ControlledPlayer const&, F const&);

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
    void addPlayerClicked();
    void removePlayerClicked();
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
      this->context<Machine>().gladeXml()
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

sc::result Configuring::react(
  events::Message<MessageType::gameStart> const& event
)
{
  context<Active>().settings(event.message.payload());
  return transit<Playing>();
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
  const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml
) :
  parent_(parent),
  window_(NULL),
  playerCombo_(NULL)
{
#define GET_WIDGET(type, name)                   \
  Gtk::type* w##name = NULL;                     \
  do {                                           \
    gladeXml->get_widget(#name, w##name);        \
    if (!w##name) {                              \
      throw std::runtime_error("missing "#name); \
    }                                            \
  } while (false)

  GET_WIDGET(Window, MainWindow);
  GET_WIDGET(TextView, MessageText);
  GET_WIDGET(Entry, ServerAddressEntry);
  GET_WIDGET(CheckButton, ReadyCheck);
  GET_WIDGET(TreeView, RemotePlayerList);
  GET_WIDGET(ComboBox, PlayerCombo);
  GET_WIDGET(Entry, PlayerNameEntry);
  GET_WIDGET(ColorButton, PlayerColorButton);
  GET_WIDGET(Button, ConnectButton);
  GET_WIDGET(Button, CreateButton);
  GET_WIDGET(Button, DeleteButton);
  GET_WIDGET(Button, AddButton);
  GET_WIDGET(Button, RemoveButton);
  GET_WIDGET(Button, UpButton);
  GET_WIDGET(Button, DownButton);
  GET_WIDGET(Button, LeftButton);
  GET_WIDGET(Button, RightButton);
  GET_WIDGET(Button, PauseButton);

  GET_WIDGET(Dialog, NewKeyDialog);
  GET_WIDGET(Button, NewKeyCancelButton);
#undef GET_WIDGET

  // Store pointers to those widgets we need to access later
  window_ = wMainWindow;
  messageView_ = wMessageText;
  readyCheck_ = wReadyCheck;
  remotePlayerList_ = wRemotePlayerList;
  playerCombo_ = wPlayerCombo;
  playerName_ = wPlayerNameEntry;
  playerColor_ = wPlayerColorButton;
  playerControlButtons_[Command::up] = wUpButton;
  playerControlButtons_[Command::down] = wDownButton;
  playerControlButtons_[Command::left] = wLeftButton;
  playerControlButtons_[Command::right] = wRightButton;
  playerControlButtons_[Command::pause] = wPauseButton;

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

  // Clear the ready check (which might be set if we've just finished a game)
  // Note that this should be done *before* the signal is connected to
  // readyCheck below.
  readyCheck_->set_active(false);

  // Connect signals from widgets to the UI
#define CONNECT_BUTTON(buttonName, memFunName)       \
  uiConnections_.push_back(                          \
    w##buttonName##Button->signal_clicked().connect( \
      sigc::mem_fun(this, &Impl::memFunName)         \
    )                                                \
  );
  CONNECT_BUTTON(Connect, connect);
  CONNECT_BUTTON(Create, createPlayer);
  CONNECT_BUTTON(Delete, deletePlayer);
  CONNECT_BUTTON(Add, addPlayerClicked);
  CONNECT_BUTTON(Remove, removePlayerClicked);
  CONNECT_BUTTON(Up, setBinding<Command::up>);
  CONNECT_BUTTON(Down, setBinding<Command::down>);
  CONNECT_BUTTON(Left, setBinding<Command::left>);
  CONNECT_BUTTON(Right, setBinding<Command::right>);
  CONNECT_BUTTON(Pause, setBinding<Command::pause>);
  CONNECT_BUTTON(NewKeyCancel, cancelNewKey);
#undef CONNECT_BUTTON
  uiConnections_.push_back(window_->signal_hide().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  ));
  uiConnections_.push_back(wReadyCheck->signal_toggled().connect(
    sigc::mem_fun(this, &Impl::readinessChange)
  ));
  uiConnections_.push_back(wPlayerColorButton->signal_color_set().connect(
    sigc::mem_fun(this, &Impl::colorChanged)
  ));
  uiConnections_.push_back(playerCombo_->signal_changed().connect(
    sigc::mem_fun(this, &Impl::refreshLocalPlayer)
  ));
  uiConnections_.push_back(playerName_->signal_changed().connect(
    sigc::mem_fun(this, &Impl::playerNameChanged)
  ));

  // Set readiness check appriopriately
  if (parent_->context<Machine>().autoReady()) {
    readyCheck_->set_active(true);
  } else {
    readyCheck_->set_active(false);
  }

  loadLocalPlayers();

  // Finally, show the GUI
  window_->show();
  refreshRemotePlayers();
}

Configuring::Impl::~Impl()
{
  saveLocalPlayers();
  // Disconnect the window hidden signal handler because we're about to hide it
  // ourselves, and we don't want to send a terminate event because of it!
  // Also all the other signal handlers because they'll contain dangling
  // pointers to this.
  while (!uiConnections_.empty()) {
    uiConnections_.back().disconnect();
    uiConnections_.pop_back();
  }
  // Similarly, clear all the TreeView and ComboBox gunk that could lead to
  // dangling pointers
  remotePlayerList_->remove_all_columns();
  remotePlayerList_->unset_model();
  playerCombo_->clear();
  playerCombo_->unset_model();
  window_->hide();
}

void Configuring::Impl::message(const std::string& message)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  buffer->insert(buffer->end(), message+"\n");
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

void Configuring::Impl::playerAdded(
  const Message<MessageType::playerAdded>& netMessage
)
{
  RemotePlayer newPlayer(netMessage.payload());
  bool inserted = remotePlayers().insert(newPlayer).second;
  if (!inserted) {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "duplicate player added"
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

Active::RemotePlayerContainer& Configuring::Impl::remotePlayers()
{
  return parent_->context<Active>().remotePlayers();
}

Active::LocalPlayerContainer& Configuring::Impl::localPlayers()
{
  return parent_->context<Active>().localPlayers();
}

ControlledPlayer const* Configuring::Impl::getCurrentPlayer()
{
  Gtk::TreeModel::iterator iter = playerCombo_->get_active();
  if (!iter)
    return NULL;
  auto& localPlayersByName = localPlayers().get<Active::NameTag>();
  std::string name = Glib::ustring((*iter)[playerComboColumns_.name_]);
  auto it = localPlayersByName.find(name);
  if (it == localPlayersByName.end()) {
    NIBBLES_FATAL("mismatch in local players");
  }
  return &*it;
}

void Configuring::Impl::addPlayerToGame(Player const& player)
{
  if (auto const& client =
      parent_->state_cast<Connectedness const&>().client()) {
    client->addPlayer(player);
  } else {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "not connected"
    );
  }
}

template<typename F>
void Configuring::Impl::modifyLocalPlayer(
  ControlledPlayer const& player,
  F const& f
)
{
  auto it = localPlayers().iterator_to(player);
  localPlayers().modify(it, f);
}

void Configuring::Impl::connected()
{
  // If players to be added automatically, then add them
  auto const& players = parent_->context<Machine>().players();
  auto const& localPlayersByName = localPlayers().get<Active::NameTag>();
  BOOST_FOREACH(auto const& playerName, players) {
    auto const it = localPlayersByName.find(playerName);
    if (it == localPlayersByName.end()) {
      parent_->context<Machine>().messageHandler().message(
        utility::Verbosity::error, "no such player '"+playerName+"'"
      );
    } else {
      addPlayerToGame(*it);
    }
  }
  // If readiness is checked, need to tell server
  if (readyCheck_->get_active()) {
    auto const& client = parent_->state_cast<Connectedness const&>().client();
    client->setReadiness(true);
  }
}

void Configuring::Impl::disconnected()
{
  remotePlayers().clear();
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
  ia >> boost::serialization::make_nvp("localPlayers", localPlayers());
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
    oa << boost::serialization::make_nvp("localPlayers", localPlayers());
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
  parent_->context<Machine>().messageHandler().message(
    utility::Verbosity::info,
    "refreshRemotePlayers: adding " +
      boost::lexical_cast<std::string>(remotePlayers().size())
  );
  BOOST_FOREACH(const RemotePlayer& player, remotePlayers()) {
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
  if (ControlledPlayer const* currentPlayer = getCurrentPlayer())
    currentName = currentPlayer->get<name>();

  // Clear out the combo box
  playerComboListStore_->clear();
  bool activeSet = false;
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers()) {
    Gtk::TreeModel::iterator iter = playerComboListStore_->append();
    Gtk::TreeModel::Row row = *iter;
    std::string name = player.get<fields::name>();
    row[playerComboColumns_.name_] = name;
    if (name == currentName) {
      playerCombo_->set_active(iter);
    } else if (currentName == "" && !activeSet) {
      // When no previous selection, select the first entry
      // (note that even if the previously selected player has the empty name,
      // this will still work because it will be set later).
      playerCombo_->set_active(iter);
      activeSet = true;
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
    for (Command c(0); c < Command::max; ++c) {
      playerControlButtons_[c]->set_label(
          c.string()+": "+gdk_keyval_name(player->get<controls>()[c])
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
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers()) {
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

  std::array<uint32_t, Command::max> newControls;
  newControls[Command::up] = GDK_Up;
  newControls[Command::down] = GDK_Down;
  newControls[Command::left] = GDK_Left;
  newControls[Command::right] = GDK_Right;
  newControls[Command::pause] = ' ';

  ControlledPlayer newPlayer(
      Player(newName, Color::yellow),
      newControls
    );
  localPlayers().push_back(newPlayer);
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

void Configuring::Impl::addPlayerClicked()
{
  Player const* currentPlayer = getCurrentPlayer();
  if (!currentPlayer) {
    parent_->context<Machine>().messageHandler().message(
      utility::Verbosity::error, "no player selected"
    );
    return;
  }
  addPlayerToGame(*currentPlayer);
}

void Configuring::Impl::removePlayerClicked()
{
  NIBBLES_FATAL("not implemented");
}

namespace {
  template<typename Field, typename Value>
  struct SetFieldImpl {
    public:
      SetFieldImpl(Value const& v) : value_(v) {}
      template<typename T>
      void operator()(T& t) const { t.template get<Field>() = value_; }
    private:
      Value const& value_;
  };

  template<typename Field, typename Value>
  SetFieldImpl<Field, Value> setField(Value const& v) { return {v}; }
}

void Configuring::Impl::playerNameChanged()
{
  std::string newName = playerName_->get_text();
  if (isNameInUse(newName)) {
    return;
  }

  if (ControlledPlayer const* currentPlayer = getCurrentPlayer()) {
    modifyLocalPlayer(*currentPlayer, setField<name>(newName));
    (*playerCombo_->get_active())[playerComboColumns_.name_] = newName;
  }
}

void Configuring::Impl::colorChanged()
{
  if (ControlledPlayer const* currentPlayer = getCurrentPlayer()) {
    auto newColor = ColorConverter::toColor(playerColor_->get_color());
    modifyLocalPlayer(*currentPlayer, setField<color>(newColor));
  }
}

void Configuring::Impl::cancelNewKey()
{
  newKeyDialog_->hide();
}

void Configuring::Impl::readinessChange()
{
  bool readiness = readyCheck_->get_active();
  if (auto* connectedness = parent_->state_cast<Connectedness const*>()) {
    if (auto const client = connectedness->client()) {
      client->setReadiness(readiness);
    }
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
  ControlledPlayer const* player = getCurrentPlayer();
  if (player) {
    // could avoid copying whole array in theory, but more code
    auto controls = player->get<fields::controls>();
    controls[Direction] = keyval;
    modifyLocalPlayer(*player, setField<fields::controls>(controls));
  }
  newKeyDialog_->hide();
  return false;
}

}}}

