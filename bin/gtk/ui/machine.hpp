#ifndef NIBBLES_GTK__UI__MACHINE_HPP
#define NIBBLES_GTK__UI__MACHINE_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>

#include <libglademm.h>

#include <nibbles/messagetype.hpp>
#include <nibbles/utility/messagehandler.hpp>
#include <nibbles/client/client.hpp>

#include "clientfactory.hpp"

namespace nibbles { namespace gtk { namespace ui {

namespace sc = boost::statechart;

namespace events {

template<MessageType::internal_enum Type>
struct Message : public sc::event<Message<Type>> {
  Message(nibbles::Message<Type> m) : message(std::move(m)) {}
  nibbles::Message<Type> message;
};

struct Terminate : public sc::event<Terminate> {};
struct Connect : public sc::event<Connect> {};
struct Disconnect : public sc::event<Disconnect> {};

}

struct MessageSinkState {
  virtual void message(std::string const&) const = 0;
};

class Active;
class Terminated;

class Machine : public sc::state_machine<Machine, Active> {
  public:
    Machine(
      utility::MessageHandler& messageHandler,
      ClientFactory& clientFactory,
      boost::filesystem::path playerFile,
      const Glib::RefPtr<Gnome::Glade::Xml>& mainXml,
      const Glib::RefPtr<Gnome::Glade::Xml>& newKeyXml
    );

    bool ended();

    utility::MessageHandler& messageHandler() {
      return messageHandler_;
    }
    ClientFactory& clientFactory() {
      return clientFactory_;
    }
    boost::filesystem::path const& playerFile() {
      return playerFile_;
    }
    Glib::RefPtr<Gnome::Glade::Xml> const& mainXml() const {
      return mainXml_;
    }
    Glib::RefPtr<Gnome::Glade::Xml> const& newKeyXml() const {
      return newKeyXml_;
    }
    boost::signal<void ()>& terminating() {
      return terminating_;
    }
  private:
    // Stuff passed down from on high
    utility::MessageHandler& messageHandler_;
    ClientFactory& clientFactory_;
    boost::filesystem::path playerFile_;
    Glib::RefPtr<Gnome::Glade::Xml> mainXml_;
    Glib::RefPtr<Gnome::Glade::Xml> newKeyXml_;

    // Signals to talk back to UI
    boost::signal<void ()> terminating_;
};

class Terminated : public sc::state<Terminated, Machine> {
  public:
    Terminated(my_context context) : my_base(context) {
      this->context<Machine>().terminating()();
    }
};

class Configuring;
class Connectedness;

class Active :
  public sc::simple_state<
    Active, Machine, boost::mpl::list<Configuring, Connectedness>
  >
{
  public:
    typedef sc::transition<events::Terminate, Terminated> reactions;
};

/// The first orthogonal component of Active follows the UI through
/// configuration and then playing the game
class Configuring :
  public sc::state<Configuring, Active::orthogonal<0>>,
  public MessageSinkState
{
  public:
    typedef boost::mpl::list<
      sc::custom_reaction<events::Message<MessageType::playerAdded>>,
      sc::custom_reaction<events::Disconnect>
    > reactions;
    Configuring(my_context);
    ~Configuring();

    virtual void message(std::string const&) const;
    sc::result react(events::Message<MessageType::playerAdded> const&);
    sc::result react(events::Disconnect const&);
  private:
    class Impl;
    boost::scoped_ptr<Impl> impl_;
};

class NotConnected;
class Connected;

/// The second orthogonal component of Active follows the network interface
class Connectedness :
  public sc::simple_state<Connectedness, Active::orthogonal<1>, NotConnected> {
  public:
    client::Client::Ptr const& client() const { return client_; }
  private:
    friend class Connected;
    friend class NotConnected;
    client::Client::Ptr client_;
};

class NotConnected :
  public sc::simple_state<NotConnected, Connectedness> {
  public:
    typedef sc::custom_reaction<events::Connect> reactions;

    sc::result react(events::Connect const&);
};

class Connected :
  public sc::simple_state<Connected, Connectedness> {
  public:
    ~Connected();

    typedef sc::transition<events::Disconnect, NotConnected> reactions;
};

}}}

#endif // NIBBLES_GTK__UI__MACHINE_HPP

