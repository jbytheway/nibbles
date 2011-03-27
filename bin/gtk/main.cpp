#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem/operations.hpp>

#include <gtkmm.h>
#include <gtkglmm.h>

#include <nibbles/client/client.hpp>

#include "gamesounds.hpp"
#include "soundservice.hpp"
#include "ui/ui.hpp"

struct IoThread {
  IoThread(boost::asio::io_service& io) :
    io_(io),
    timer_(io, boost::posix_time::seconds(1))
  {}
  boost::asio::io_service& io_;
  boost::asio::deadline_timer timer_;
  void operator()() {
    timer_.async_wait(boost::bind(
          &IoThread::timerExpired, this, boost::asio::placeholders::error
        ));
    io_.run();
    std::cout << "IO thread completed" << std::endl;
  }
  void timerExpired(const boost::system::error_code& e) {
    if (e) {
      std::cout << "IO thread interrupted" << std::endl;
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

  boost::filesystem::path exePath(argv[0]);
  if (!exePath.has_root_path()) {
    exePath = cwd/exePath;
  }
  exePath = exePath.parent_path();

  // Cut out libtool .libs dir
  if (exePath.filename() == ".libs") {
    exePath = exePath.parent_path();
  }

  boost::asio::io_service io;
  Gtk::Main kit(argc, argv);
  Gtk::GL::init(argc, argv);
  const nibbles::gtk::Options options(argc, argv);
  if (options.help) {
    options.show_help(std::cout);
    return 0;
  }

  boost::filesystem::path gladeDir = options.gladePath;

  if (gladeDir.empty()) {
    gladeDir = exePath;
  }

  boost::filesystem::path soundDir = options.soundPath;

  if (soundDir.empty()) {
    soundDir = exePath;
  }

  auto fontPath = options.fontPath;

  if (fontPath.empty()) {
    fontPath = exePath/"Courier.ttf";
  }

  auto gladeFile = gladeDir/"nibbles.glade";
  Glib::RefPtr<Gnome::Glade::Xml> gladeXml =
    Gnome::Glade::Xml::create(gladeFile.string());

  nibbles::gtk::SoundService service;
  nibbles::gtk::GameSounds sounds(service, soundDir);
  sounds.intro->asyncPlay();

  nibbles::gtk::ui::UI ui(io, options, gladeXml, fontPath, sounds);

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

