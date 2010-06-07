#include "ui.hpp"

#include <gtkmm.h>

#include <nibbles/fatal.hpp>
#include <nibbles/direction.hpp>
#include <nibbles/message.hpp>
#include <nibbles/client/client.hpp>

#include "crossthreadsignal.hpp"
#include "machine.hpp"
#include "remoteplayer.hpp"

namespace nibbles { namespace gtk { namespace ui {

class UI::Impl :
  public utility::MessageHandler,
  public ClientFactory
{
  public:
    Impl(
        boost::asio::io_service& io,
        const Options&,
        const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml,
        const Glib::RefPtr<Gnome::Glade::Xml>& playXml
      );

    bool ended() { return machine_.ended(); }
    virtual void message(utility::Verbosity, const std::string& message);
    virtual client::Client::Ptr makeClient();
  private:
    void shutdown();

    boost::asio::io_service& io_;
    Options options_;
    Machine machine_;

    CrossThreadSignal<std::string> messageSignal_;
    void writeMessage(const std::string&); // Call only on GUI thread

    CrossThreadSignal<MessageBase::Ptr> netMessageSignal_;
    void handleNetMessage(const MessageBase::Ptr&); // Call only on GUI thread

    CrossThreadSignal<> disconnectSignal_;
    void disconnect();

    Gtk::Window* playWindow_;
    Gtk::DrawingArea* levelDisplay_;
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
  machine_(*this, *this, options.playerFile, mainXml, newKeyXml)
{
  // Connect the message alert signals to our functions
  messageSignal_.connect(sigc::mem_fun(this, &Impl::writeMessage));
  netMessageSignal_.connect(sigc::mem_fun(this, &Impl::handleNetMessage));
  disconnectSignal_.connect(sigc::mem_fun(this, &Impl::disconnect));
  // Detect termination so we can shut down the event loop
  machine_.terminating().connect(sigc::mem_fun(this, &Impl::shutdown));

#define GET_WIDGET(xml, type, name)              \
  Gtk::type* w##name = NULL;                     \
  do {                                           \
    xml##Xml->get_widget(#name, w##name);        \
    if (!w##name) {                              \
      throw std::runtime_error("missing "#name); \
    }                                            \
  } while (false)

  GET_WIDGET(play, Window, PlayWindow);
  GET_WIDGET(play, DrawingArea, LevelDisplay);
#undef GET_WIDGET

  playWindow_ = wPlayWindow;
  levelDisplay_ = wLevelDisplay;

  machine_.initiate();
}

void UI::Impl::message(utility::Verbosity v, const std::string& message)
{
  if (options_.verbosity <= v) {
    messageSignal_(message);
  }
}

client::Client::Ptr UI::Impl::makeClient()
{
  try {
    auto client =
      nibbles::client::Client::create(
          io_, *this, options_.protocol, options_.address, options_.port
        );
    client->messageSignal().connect(boost::ref(netMessageSignal_));
    client->terminateSignal().connect(boost::ref(disconnectSignal_));
    client->connect();
    return client;
  } catch (boost::system::system_error& e) {
    message(
      utility::Verbosity::error,
      std::string("connection failed: ")+e.what()+"\n"
    );
    throw;
  }
}

void UI::Impl::writeMessage(const std::string& message)
{
  if (MessageSinkState const* messageSink =
    machine_.state_cast<MessageSinkState const*>()) {
    messageSink->message(message);
  }
}

void UI::Impl::handleNetMessage(const MessageBase::Ptr& message)
{
  switch (message->type()) {
#define CASE(r, d, value)                                          \
    case MessageType::value:                                       \
      machine_.process_event(events::Message<MessageType::value>(  \
        dynamic_cast<const Message<MessageType::value>&>(*message) \
      ));                                                          \
      return;
    BOOST_PP_SEQ_FOR_EACH(CASE, _, NIBBLES_MESSAGETYPE_VALUES())
#undef CASE
    default:
      NIBBLES_FATAL("unknown MessageType");
  }
}

void UI::Impl::disconnect()
{
  machine_.process_event(events::Disconnect());
}

void UI::Impl::shutdown()
{
  if (options_.threaded) {
    Gtk::Main::quit();
  }
}

}}}

