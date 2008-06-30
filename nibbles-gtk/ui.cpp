#include "ui.hpp"

#include <nibbles/direction.hpp>

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
    const Glib::RefPtr<Gnome::Glade::Xml>& refXml
  ) :
  io_(io),
  options_(options),
  window_(NULL),
  playerCombo_(NULL)
{
#define GET_WIDGET(type, name)                   \
  Gtk::type* w##name = NULL;                     \
  do {                                           \
    refXml->get_widget(#name, w##name);          \
    if (!w##name) {                              \
      throw std::runtime_error("missing "#name); \
    }                                            \
  } while (false)

  GET_WIDGET(Window, MainWindow);
  GET_WIDGET(TextView, MessageText);
  GET_WIDGET(Entry, ServerAddressEntry);
  GET_WIDGET(CheckButton, ReadyCheck);
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

  // Store pointers to those widgets we need to access later
  window_ = wMainWindow;
  messages_ = wMessageText->get_buffer();
  playerCombo_ = wPlayerCombo;

  // Attach the columns to their controls
  playerComboListStore_ = Gtk::ListStore::create(playerComboColumns_);
  assert(playerComboListStore_);
  playerCombo_->set_model(playerComboListStore_);
  playerCombo_->pack_start(playerComboColumns_.name_);
  
  // Connect signals from widgets to the UI
#define CONNECT_BUTTON(buttonName, memFunName)    \
  w##buttonName##Button->signal_clicked().connect( \
      sigc::mem_fun(this, &UI::memFunName)        \
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

  loadLocalPlayers();
}

UI::~UI()
{
  saveLocalPlayers();
}

void UI::message(utility::Verbosity v, const std::string& message)
{
  if (options_.verbosity <= v) {
    messages_->insert(messages_->end(), message);
  }
}

Player* UI::getCurrentPlayer()
{
  Gtk::TreeModel::iterator iter = playerCombo_->get_active();
  if (!iter)
    return NULL;
  size_t index = iter - playerComboListStore_->children().begin();
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
  path playerFileDir = playerFilePath.branch_path();
  if (!exists(playerFileDir)) {
    message(Verbosity::info, "creating directory "+playerFileDir.string());
    create_directories(playerFileDir);
  }
  boost::filesystem::ofstream ofs(playerFilePath);
  boost::archive::xml_oarchive oa(ofs);
  oa << BOOST_SERIALIZATION_NVP(localPlayers_);
}

void UI::refreshPlayers()
{
  // Save the current player so we can put things back properly afterwards
  string currentName;
  if (Player* currentPlayer = getCurrentPlayer())
    currentName = currentPlayer->get<name>();

  // Clear out the combo box
  playerComboListStore_->clear();
  BOOST_FOREACH(const Player& player, localPlayers_) {
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
  // TODO:
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

void UI::createPlayer()
{
  string newName = "New player";
  BOOST_FOREACH(const Player& player, localPlayers_) {
    if (player.get<name>() == newName) {
      message(Verbosity::error, "name '"+newName+"' in use");
      return;
    }
  }

  localPlayers_.push_back(Player(newName, Color::yellow));
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
  throw logic_error("not implemented");
}

void UI::removePlayerFromGame()
{
  throw logic_error("not implemented");
}

template<int Direction>
void UI::setBinding()
{
  throw logic_error("not implemented");
}

}}

