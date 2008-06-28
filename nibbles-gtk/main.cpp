#include <iostream>

#include <boost/asio.hpp>

#include <nibbles/client/client.hpp>

#include "ui.hpp"

using namespace std;
using namespace boost::asio;
using namespace nibbles::gtk;

int main(int argc, char** argv)
{
  io_service io;
  Gtk::Main kit(argc, argv);
  const Options options(argc, argv);
  Glib::RefPtr<Gnome::Glade::Xml> refXml =
    Gnome::Glade::Xml::create("nibbles.glade");

  UI ui(io, options, refXml);
  Gtk::Main::run(ui.window());
  return 0;
}

