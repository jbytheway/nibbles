#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <nibbles/client/client.hpp>

#include "ui.hpp"

using namespace std;
using namespace boost::asio;
using namespace nibbles::gtk;

struct IoThread {
  IoThread(io_service& io) :
    io_(io),
    timer_(io, boost::posix_time::seconds(1))
  {}
  io_service& io_;
  deadline_timer timer_;
  void operator()() {
    timer_.async_wait(boost::bind(
          &IoThread::timerExpired, this, boost::asio::placeholders::error
        ));
    io_.run();
    cout << "IO thread completed" << endl;
  }
  void timerExpired(const boost::system::error_code& e) {
    if (e) {
      cout << "IO thread interrupted" << endl;
    } else {
      timer_.expires_from_now(boost::posix_time::seconds(1));
      timer_.async_wait(boost::bind(
            &IoThread::timerExpired, this, boost::asio::placeholders::error
          ));
    }
  }
  void interrupt() {
    timer_.cancel();
  }
};

int main(int argc, char** argv)
{
  io_service io;
  Gtk::Main kit(argc, argv);
  const Options options(argc, argv);
  Glib::RefPtr<Gnome::Glade::Xml> refXml =
    Gnome::Glade::Xml::create("nibbles.glade");

  UI ui(io, options, refXml);
  IoThread ioThreadObj(io);
  boost::thread ioThread(boost::ref(ioThreadObj));
  Gtk::Main::run(ui.window());
  ioThreadObj.interrupt();
  ioThread.join();
  return 0;
}

