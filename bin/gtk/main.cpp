#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem/operations.hpp>

#include <gtkmm.h>

#include <nibbles/client/client.hpp>

#include "ui/ui.hpp"

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
  auto cwd = boost::filesystem::initial_path();

  io_service io;
  Gtk::Main kit(argc, argv);
  const Options options(argc, argv);
  if (options.help) {
    options.show_help(std::cout);
    return 0;
  }

  boost::filesystem::path gladeDir = options.gladePath;

  if (gladeDir.empty()) {
    boost::filesystem::path exe(argv[0]);
    if (!exe.has_root_path()) {
      exe = cwd/exe;
    }
    gladeDir = exe.parent_path();
  }

  auto gladeFile = gladeDir/"nibbles.glade";
  Glib::RefPtr<Gnome::Glade::Xml> gladeXml =
    Gnome::Glade::Xml::create(gladeFile.file_string());

  ui::UI ui(io, options, gladeXml);

  if (options.threaded) {
    IoThread ioThreadObj(io);
    boost::thread ioThread(boost::ref(ioThreadObj));
    Gtk::Main::run();
    ioThreadObj.interrupt();
    ioThread.join();
  } else {
    // FIXME: This polling version is very inefficient (most CPU time spent
    // spinlocking)
    //struct timespec sleepTime = { 0, 0 };
    while (!ui.ended()) {
      Gtk::Main::iteration(false);
      io.poll();
      io.reset();
      // Even nanosleeping for 0 time sleeps too long because it actually
      // sleeps for ~10ms
      //nanosleep(&sleepTime, NULL);
    }
    io.run();
  }
  return 0;
}

