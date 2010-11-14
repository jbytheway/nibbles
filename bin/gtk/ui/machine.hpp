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
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <libglademm.h>

#include <nibbles/messagetype.hpp>
#include <nibbles/utility/messagehandler.hpp>
#include <nibbles/client/client.hpp>

#include "clientfactory.hpp"
#include "remoteplayer.hpp"
#include "controlledplayer.hpp"

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
struct Connected : public sc::event<Connected> {};
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
      const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml
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
    Glib::RefPtr<Gnome::Glade::Xml> const& gladeXml() const {
      return gladeXml_;
    }
    boost::signal<void ()>& terminating() {
      return terminating_;
    }

    // For debugging
    void dump();
  private:
    // Stuff passed down from on high
    utility::MessageHandler& messageHandler_;
    ClientFactory& clientFactory_;
    boost::filesystem::path playerFile_;
    Glib::RefPtr<Gnome::Glade::Xml> gladeXml_;

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
    typedef boost::mpl::list<
#define REACTION(r, _, type) \
      sc::custom_reaction<events::Message<MessageType::type>>,
      BOOST_PP_SEQ_FOR_EACH(REACTION, _, NIBBLES_MESSAGETYPE_VALUES())
#undef REACTION
      sc::transition<events::Terminate, Terminated>
    > reactions;

    template<MessageType::internal_enum Type>
    sc::result react(events::Message<Type> const&) {
      std::string message =
        "unhandled message of type "+MessageType(Type).string()+"\n";
      for (state_iterator leaf = state_begin();
          leaf != state_end(); ++leaf) {
        message += typeid(*leaf).name();
        message += " ";
      }
      message += "\n";

      context<Machine>().messageHandler().message(
        utility::Verbosity::warning, message
      );
      return forward_event();
    }

    class NameTag;
    class SequenceTag;
    typedef boost::multi_index_container<
        RemotePlayer,
        boost::multi_index::indexed_by<
          boost::multi_index::ordered_unique<
            BOOST_MULTI_INDEX_CONST_MEM_FUN(
                IdedPlayer::base, const PlayerId&, get<id>
              )
          >,
          boost::multi_index::ordered_unique<
            boost::multi_index::tag<NameTag>,
            BOOST_MULTI_INDEX_CONST_MEM_FUN(
                IdedPlayer::base, const std::string&, get<name>
              )
          >,
          boost::multi_index::sequenced<
            boost::multi_index::tag<SequenceTag>
          >
        >
      > RemotePlayerContainer;
    RemotePlayerContainer& remotePlayers() { return remotePlayers_; }
    RemotePlayerContainer const& remotePlayers() const {
      return remotePlayers_;
    }
    std::vector<ControlledPlayer>& localPlayers() { return localPlayers_; }
    std::vector<ControlledPlayer> const& localPlayers() const {
      return localPlayers_;
    }
  private:
    RemotePlayerContainer remotePlayers_;
    std::vector<ControlledPlayer> localPlayers_;
};

/// The first orthogonal component of Active follows the UI through
/// configuration and then playing the game
class Playing;

class Configuring :
  public sc::state<Configuring, Active::orthogonal<0>>,
  public MessageSinkState
{
  public:
    typedef boost::mpl::list<
      sc::custom_reaction<events::Message<MessageType::playerAdded>>,
      sc::custom_reaction<events::Message<MessageType::updateReadiness>>,
      sc::custom_reaction<events::Connected>,
      sc::custom_reaction<events::Disconnect>,
      sc::transition<events::Message<MessageType::gameStart>, Playing>
    > reactions;
    Configuring(my_context);
    ~Configuring();

    virtual void message(std::string const&) const;
    sc::result react(events::Message<MessageType::playerAdded> const&);
    sc::result react(events::Message<MessageType::updateReadiness> const&);
    sc::result react(events::Connected const&);
    sc::result react(events::Disconnect const&);
  private:
    class Impl;
    boost::scoped_ptr<Impl> impl_;
};

class Playing :
  public sc::state<Playing, Active::orthogonal<0>>,
  public MessageSinkState
{
  public:
    typedef boost::mpl::list<
      sc::custom_reaction<events::Message<MessageType::levelStart>>,
      sc::custom_reaction<events::Message<MessageType::newNumber>>,
      sc::custom_reaction<events::Message<MessageType::tick>>
    > reactions;

    Playing(my_context);
    ~Playing();

    virtual void message(std::string const&) const;
    sc::result react(events::Message<MessageType::levelStart> const&);
    sc::result react(events::Message<MessageType::newNumber> const&);
    sc::result react(events::Message<MessageType::tick> const&);
  private:
    class Impl;
    boost::scoped_ptr<Impl> impl_;
};

/// The second orthogonal component of Active follows the network interface
class NotConnected;
class Connected;

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
  public sc::state<Connected, Connectedness> {
  public:
    Connected(my_context);
    ~Connected();

    typedef sc::transition<events::Disconnect, NotConnected> reactions;
};

}}}

#endif // NIBBLES_GTK__UI__MACHINE_HPP

