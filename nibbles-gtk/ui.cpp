#include "ui.hpp"

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
  options_(options)
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
  GET_WIDGET(Entry, PlayerNameEntry);
  GET_WIDGET(ColorButton, PlayerColorButton);
  GET_WIDGET(Button, ConnectButton);
  GET_WIDGET(Button, CreateButton);
  GET_WIDGET(Button, AddButton);
  GET_WIDGET(Button, RemoveButton);
  GET_WIDGET(Button, UpButton);
  GET_WIDGET(Button, DownButton);
  GET_WIDGET(Button, LeftButton);
  GET_WIDGET(Button, RightButton);

  // Store pointers to those widgets we need to access later
  window_ = wMainWindow;
  messages_ = wMessageText->get_buffer();
  // Connect signals from widgets to the UI
  wConnectButton->signal_clicked().connect(sigc::mem_fun(this, &UI::connect));
}

void UI::message(utility::Verbosity v, const std::string& message)
{
  if (options_.verbosity <= v) {
    messages_->insert(messages_->end(), message);
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

}}

