#include <boost/asio.hpp>
#include <gtkmm.h>
#include <libglademm.h>

#include <nibbles/client/client.hpp>

#include "ui.hpp"

using namespace boost::asio;
using namespace nibbles::client;
using namespace nibbles::gtk;

int main(int argc, char** argv)
{
  io_service io;
  Gtk::Main(argc, argv);
  const Options options(argc, argv);
  Client::Ptr client =
    Client::create(io, options.protocol, options.address, options.port);
  Glib::RefPtr<Gnome::Glade::Xml> refXml =
    Gnome::Glade::Xml::create("nibbles++.glade");

#define GET_WIDGET(type, name, variable)       \
  Gtk::type* variable = NULL;                  \
  refXml->get_widget(#name, variable);         \
  if (!variable) {                             \
    throw std::runtime_error("missing "#name); \
  }

  GET_WIDGET(Window, MainWindow, window)

  UI ui(options);
  // TODO: connect signals from both ends to the UI
  client->connect();
  Gtk::Main::run(*window);
  return 0;
}

