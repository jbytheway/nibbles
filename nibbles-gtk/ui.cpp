#include "ui.hpp"

#include "colorconverter.hpp"

#include <nibbles/direction.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::system;
using namespace nibbles::utility;
using namespace nibbles::client;

namespace nibbles { namespace gtk {

UI::UI(
    boost::asio::io_service& io,
    const Options& options,
    const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
    const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml
  ) :
  io_(io),
  options_(options),
  window_(NULL),
  playerCombo_(NULL)
{
  // Connect the message alert signal to our function
  messageAlert_.connect(sigc::mem_fun(this, &UI::writeMessage));

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
  playerComboListStore_ = Gtk::ListStore::create(playerComboColumns_);
  assert(playerComboListStore_);
  playerCombo_->set_model(playerComboListStore_);
  playerCombo_->pack_start(playerComboColumns_.name_);
  
  // Connect signals from widgets to the UI
#define CONNECT_BUTTON(buttonName, memFunName)     \
  w##buttonName##Button->signal_clicked().connect( \
      sigc::mem_fun(this, &UI::memFunName)         \
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
  wPlayerColorButton->signal_color_set().connect(
      sigc::mem_fun(this, &UI::colorChanged)
    );
  playerCombo_->signal_changed().connect(
      sigc::mem_fun(this, &UI::refreshPlayer)
    );
  playerName_->signal_changed().connect(
      sigc::mem_fun(this, &UI::playerNameChanged)
    );

  loadLocalPlayers();
}

UI::~UI()
{
  saveLocalPlayers();
}

void UI::message(utility::Verbosity v, const std::string& message)
{
  if (options_.verbosity <= v) {
    boost::lock_guard<boost::mutex> lock(messagesMutex_);
    messages_.push_back(message);
    messageAlert_();
  }
}

void UI::writeMessage()
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  boost::lock_guard<boost::mutex> lock(messagesMutex_);
  while (!messages_.empty()) {
    buffer->insert(buffer->end(), messages_.front());
    messages_.pop_front();
  }
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

ControlledPlayer* UI::getCurrentPlayer()
{
  Gtk::TreeModel::iterator iter = playerCombo_->get_active();
  if (!iter)
    return NULL;
  // TODO: Do we need a faster-than-linear implementation?
  size_t index = std::distance(playerComboListStore_->children().begin(), iter);
  assert(index < localPlayers_.size());
  return &localPlayers_[index];
}

void UI::loadLocalPlayers()
{
  using namespace boost::filesystem;
  path playerFilePath(options_.playerFile);
  if (!exists(playerFilePath)) {
    message(Verbosity::info, "no player file at "+options_.playerFile);
    return;
  }
  boost::filesystem::ifstream ifs(playerFilePath);
  boost::archive::xml_iarchive ia(ifs);
  ia >> BOOST_SERIALIZATION_NVP(localPlayers_);
  refreshPlayers();
}

void UI::saveLocalPlayers()
{
  using namespace boost::filesystem;
  path playerFilePath(options_.playerFile);
  path tempPlayerFilePath(options_.playerFile+".new");
  path playerFileDir = playerFilePath.branch_path();
  if (!exists(playerFileDir)) {
    message(Verbosity::info, "creating directory "+playerFileDir.string());
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

void UI::refreshPlayers()
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

  refreshPlayer();
}

void UI::refreshPlayer()
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

void UI::connect()
{
  client_ =
    Client::create(
        io_, *this, options_.protocol, options_.address, options_.port
      );
  try {
    client_->connect();
  } catch (system_error& e) {
    message(Verbosity::error, string("connection failed: ")+e.what()+"\n");
    client_.reset();
  }
}

bool UI::nameInUse(const string& name)
{
  // TODO: Do we need a faster-than-linear implementation?
  BOOST_FOREACH(const ControlledPlayer& player, localPlayers_) {
    if (player.get<fields::name>() == name) {
      return true;
    }
  }

  return false;
}

void UI::createPlayer()
{
  string newName = "New player";
  
  if (nameInUse(newName)) {
    message(Verbosity::error, "name '"+newName+"' in use");
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

  refreshPlayer();
}

void UI::deletePlayer()
{
  throw logic_error("not implemented");
}

void UI::addPlayerToGame()
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

void UI::removePlayerFromGame()
{
  throw logic_error("not implemented");
}

void UI::playerNameChanged()
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

void UI::colorChanged()
{
  if (Player* currentPlayer = getCurrentPlayer()) {
    currentPlayer->get<color>() =
      ColorConverter::toColor(playerColor_->get_color());
  }
}

void UI::cancelNewKey()
{
  newKeyDialog_->hide();
}

template<int Direction>
void UI::setBinding()
{
  if (!getCurrentPlayer())
    return;
  sigc::connection connection0 =
    newKeyDialog_->signal_key_press_event().connect(
      sigc::mem_fun(this, &UI::newKey<Direction>)
    );
  sigc::connection connection1 =
    newKeyCancelButton_->signal_key_press_event().connect(
      sigc::mem_fun(this, &UI::newKey<Direction>)
    );
  newKeyDialog_->run();
  // Bizarrely, if close button clicked, doesn't hide but does return from
  // run(), so we hide manually
  newKeyDialog_->hide();
  message(Verbosity::info, "new key dialog closed\n");
  connection0.disconnect();
  connection1.disconnect();
  refreshPlayer();
}

template<int Direction>
bool UI::newKey(GdkEventKey* event)
{
  uint32_t keyval = event->keyval;
  ControlledPlayer* player = getCurrentPlayer();
  if (player)
    player->get<controls>()[Direction] = keyval;
  newKeyDialog_->hide();
  return false;
}

}}

