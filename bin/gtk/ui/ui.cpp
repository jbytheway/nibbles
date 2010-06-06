#include "ui.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/foreach.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <gtkmm.h>

#include <nibbles/fatal.hpp>
#include <nibbles/direction.hpp>
#include <nibbles/message.hpp>
#include <nibbles/client/client.hpp>

#include "colorconverter.hpp"
#include "crossthreadsignal.hpp"
#include "remoteplayer.hpp"
#include "controlledplayer.hpp"

using namespace std;
using namespace boost::system;
using namespace nibbles::utility;
using namespace nibbles::client;

namespace nibbles { namespace gtk { namespace ui {

class UI::Impl : public utility::MessageHandler {
  public:
    Impl(
        boost::asio::io_service& io,
        const Options&,
        const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& playXml
      );
    ~Impl();

    bool ended() { return !window_->is_visible(); }
    virtual void message(utility::Verbosity, const std::string& message);
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
    void windowClosed();
    void connect();
    void disconnect(); // not really a UI binding
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

UI::UI(
    boost::asio::io_service& io,
    const Options& options,
    const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
    const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml,
    const Glib::RefPtr<Gnome::Glade::Xml>& playXml
  ) :
  impl_(new Impl(io, options, mainXml, newKeyXml, playXml))
{}

UI::~UI() = default;

bool UI::ended() { return impl_->ended(); }

UI::Impl::Impl(
    boost::asio::io_service& io,
    const Options& options,
    const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
    const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml,
    const Glib::RefPtr<Gnome::Glade::Xml>& playXml
  ) :
  io_(io),
  options_(options),
  window_(NULL),
  playerCombo_(NULL)
{
  // Connect the message alert signals to our functions
  messageSignal_.connect(sigc::mem_fun(this, &Impl::writeMessage));
  netMessageSignal_.connect(sigc::mem_fun(this, &Impl::handleNetMessage));

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

  GET_WIDGET(play, Window, PlayWindow);
  GET_WIDGET(play, DrawingArea, LevelDisplay);
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

  playWindow_ = wPlayWindow;
  levelDisplay_ = wLevelDisplay;

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

UI::Impl::~Impl()
{
  saveLocalPlayers();
}

void UI::Impl::message(utility::Verbosity v, const std::string& message)
{
  if (options_.verbosity <= v) {
    messageSignal_(message);
  }
}

void UI::Impl::writeMessage(const string& message)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  buffer->insert(buffer->end(), message);
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

#define IGNORE_MESSAGE(type)                                 \
template<>                                                   \
void UI::Impl::internalNetMessage<MessageType::type>(              \
    const Message<MessageType::type>&                        \
  )                                                          \
{                                                            \
  message(Verbosity::warning, "ignoring "#type" message\n"); \
}

IGNORE_MESSAGE(addPlayer)
IGNORE_MESSAGE(setReadiness)

#undef IGNORE_MESSAGE

template<>
void UI::Impl::internalNetMessage<MessageType::playerAdded>(
    const Message<MessageType::playerAdded>& netMessage
  )
{
  RemotePlayer newPlayer(netMessage.payload(), false);
  bool inserted = remotePlayers_.insert(newPlayer).second;
  if (!inserted) {
    message(Verbosity::error, "duplicate player added\n");
  }
  refreshRemotePlayers();
}

template<>
void UI::Impl::internalNetMessage<MessageType::updateReadiness>(
    const Message<MessageType::updateReadiness>& netMessage
  )
{
  ClientId id = netMessage.payload().first;
  bool ready = netMessage.payload().second;
  typedef RemotePlayerContainer::index<SequenceTag>::type Index;
  Index& index = remotePlayers_.get<SequenceTag>();

  for (Index::iterator i = index.begin(); i != index.end(); ++i) {
    if (i->get<clientId>() == id) {
      index.replace(i, RemotePlayer(*i, ready));
    }
  }
  refreshRemotePlayers();
}

template<>
void UI::Impl::internalNetMessage<MessageType::gameStart>(
    const Message<MessageType::gameStart>& /*netMessage*/
  )
{
  NIBBLES_FATAL("not implemented");
}

void UI::Impl::handleNetMessage(const MessageBase::Ptr& message)
{
  switch (message->type()) {
#define CASE(r, d, value)                                            \
    case MessageType::value:                                         \
      internalNetMessage<MessageType::value>(                        \
          dynamic_cast<const Message<MessageType::value>&>(*message) \
        );                                                           \
      return;
    BOOST_PP_SEQ_FOR_EACH(CASE, _, NIBBLES_MESSAGETYPE_VALUES())
#undef CASE
    default:
      NIBBLES_FATAL("unknown MessageType");
  }
}

ControlledPlayer* UI::Impl::getCurrentPlayer()
{
  Gtk::TreeModel::iterator iter = playerCombo_->get_active();
  if (!iter)
    return NULL;
  // TODO: Do we need a faster-than-linear implementation?
  size_t index = std::distance(playerComboListStore_->children().begin(), iter);
  assert(index < localPlayers_.size());
  return &localPlayers_[index];
}

void UI::Impl::loadLocalPlayers()
{
  using namespace boost::filesystem;
  path playerFilePath(options_.playerFile);
  if (!exists(playerFilePath)) {
    message(Verbosity::info, "no player file at "+options_.playerFile+"\n");
    return;
  }
  boost::filesystem::ifstream ifs(playerFilePath);
  boost::archive::xml_iarchive ia(ifs);
  ia >> BOOST_SERIALIZATION_NVP(localPlayers_);
  refreshLocalPlayers();
}

void UI::Impl::saveLocalPlayers()
{
  using namespace boost::filesystem;
  path playerFilePath(options_.playerFile);
  path tempPlayerFilePath(options_.playerFile+".new");
  path playerFileDir = playerFilePath.branch_path();
  if (!exists(playerFileDir)) {
    message(Verbosity::info, "creating directory "+playerFileDir.string()+"\n");
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

void UI::Impl::refreshRemotePlayers()
{
  // Save the current player so we can put things back properly afterwards
  PlayerId currentId = PlayerId::invalid();
  if (Gtk::TreeModel::iterator iter =
      remotePlayerList_->get_selection()->get_selected())
    currentId = PlayerId::fromInteger((*iter)[remotePlayerListColumns_.id_]);

  // Clear out the list box
  remotePlayerListStore_->clear();
  BOOST_FOREACH(const RemotePlayer& player, remotePlayers_) {
    Gtk::TreeModel::iterator iter = remotePlayerListStore_->append();
    Gtk::TreeModel::Row row = *iter;
    row[remotePlayerListColumns_.ready_] = player.get<ready>();
    row[remotePlayerListColumns_.id_] = player.get<id>();
    row[remotePlayerListColumns_.clientId_] = player.get<clientId>();
    row[remotePlayerListColumns_.color_] =
      ColorConverter::toGdkColor(player.get<color>());
    row[remotePlayerListColumns_.name_] = player.get<name>();
    if (player.get<id>() == currentId) {
      remotePlayerList_->get_selection()->select(iter);
    }
  }
}

void UI::Impl::refreshLocalPlayers()
{
  // Save the current player so we can put things back properly afterwards
  string currentName;
  if (ControlledPlayer* currentPlayer = getCurrentPlayer())
    currentName = currentPlayer->get<name>();

  // Clear out the combo box
  playerComboListStore_->clear();
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers_) {
    Gtk::TreeModel::iterator iter = playerComboListStore_->append();
    Gtk::TreeModel::Row row = *iter;
    string name = player.get<fields::name>();
    row[playerComboColumns_.name_] = name;
    if (name == currentName) {
      playerCombo_->set_active(iter);
    }
  }

  refreshLocalPlayer();
}

void UI::Impl::refreshLocalPlayer()
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

void UI::Impl::windowClosed()
{
  if (options_.threaded) {
    Gtk::Main::quit();
  }
}

void UI::Impl::connect()
{
  client_ =
    Client::create(
        io_, *this, options_.protocol, options_.address, options_.port
      );
  try {
    client_->messageSignal().connect(boost::ref(netMessageSignal_));
    client_->terminateSignal().connect(
        boost::bind(&Impl::disconnect, this)
      );
    client_->connect();
  } catch (system_error& e) {
    message(Verbosity::error, string("connection failed: ")+e.what()+"\n");
    client_.reset();
  }
}

void UI::Impl::disconnect()
{
  message(Verbosity::error, "disconnecting\n");
  if (client_) {
    remotePlayers_.clear();
    client_->close();
    client_.reset();
    refreshRemotePlayers();
  }
}

bool UI::Impl::nameInUse(const string& name)
{
  // TODO: Do we need a faster-than-linear implementation?
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers_) {
    if (player.get<fields::name>() == name) {
      return true;
    }
  }

  return false;
}

void UI::Impl::createPlayer()
{
  string newName = "New player";

  if (nameInUse(newName)) {
    message(Verbosity::error, "name '"+newName+"' in use\n");
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

void UI::Impl::deletePlayer()
{
  NIBBLES_FATAL("not implemented");
}

void UI::Impl::addPlayerToGame()
{
  Player* currentPlayer = getCurrentPlayer();
  if (!currentPlayer) {
    message(Verbosity::error, "no player selected\n");
    return;
  }
  if (!client_) {
    message(Verbosity::error, "not connected\n");
    return;
  }
  client_->addPlayer(*currentPlayer);
}

void UI::Impl::removePlayerFromGame()
{
  NIBBLES_FATAL("not implemented");
}

void UI::Impl::playerNameChanged()
{
  string newName = playerName_->get_text();
  if (nameInUse(newName)) {
    return;
  }

  if (Player* currentPlayer = getCurrentPlayer()) {
    currentPlayer->get<name>() = newName;
    (*playerCombo_->get_active())[playerComboColumns_.name_] = newName;
  }
}

void UI::Impl::colorChanged()
{
  if (Player* currentPlayer = getCurrentPlayer()) {
    currentPlayer->get<color>() =
      ColorConverter::toColor(playerColor_->get_color());
  }
}

void UI::Impl::cancelNewKey()
{
  newKeyDialog_->hide();
}

void UI::Impl::readinessChange()
{
  bool readiness = readyCheck_->get_active();
  client_->setReadiness(readiness);
}

template<int Direction>
void UI::Impl::setBinding()
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
  message(Verbosity::debug, "new key dialog closed\n");
  connection0.disconnect();
  connection1.disconnect();
  refreshLocalPlayer();
}

template<int Direction>
bool UI::Impl::newKey(GdkEventKey* event)
{
  uint32_t keyval = event->keyval;
  ControlledPlayer* player = getCurrentPlayer();
  if (player)
    player->get<controls>()[Direction] = keyval;
  newKeyDialog_->hide();
  return false;
}

}}}

