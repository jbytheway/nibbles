#include "server.hpp"

#include "signalcatcher.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/xml_iarchive.hpp>

using namespace std;
using namespace boost::asio;
using namespace nibbles::utility;

namespace nibbles { namespace server {

Server::Server(io_service& io, ostream& out, const Options& o) :
  io_(io),
  out_(out),
  options_(o),
  tcp_(*this)
{
  signalCatcher.connect(boost::bind(&Server::signalled, this));

  boost::filesystem::path levelsFile(o.levelPack);
  if (levelsFile.empty()) {
    throw runtime_error("no levelpack specified");
  }
  boost::filesystem::ifstream ifs(levelsFile);
  boost::archive::xml_iarchive ia(ifs);
  ia >> BOOST_SERIALIZATION_NVP(levelPack_);
}

void Server::serve()
{
  if (options_.useTcp) {
    ip::address addr(ip::address::from_string(options_.tcpAddress));
    tcp_.serve(ip::tcp::endpoint(addr, options_.tcpPort));
  }

  io_.run(); // Blocks until all stuff is done
  message(Verbosity::info, "server done\n");
}

void Server::addConnection(const Connection::Ptr& connection)
{
  if (game_.started()) {
    message(Verbosity::error, "connection failed; game started\n");
    connection->close();
    return;
  }

  connection->messageSignal.connect(
      boost::bind(&Server::netMessage, this, _1, _2)
    );
  connection->terminateSignal.connect(
      boost::bind(&Server::deleteConnection, this, _1)
    );
  connection->setId(nextClientId_++);
  connection->start();
  bool inserted = connectionPool_.insert(connection).second;
  if (inserted) {
    message(
        Verbosity::info,
        "added connection; "+
        boost::lexical_cast<string>(connectionPool_.size())+" now exist\n"
      );
  } else {
    message(Verbosity::error, "connection failed; id in use");
    connection->close();
  }
}

void Server::message(Verbosity v, const string& message)
{
  if (options_.verbosity <= v)
    out_ << message << flush;
}

// Server::netMessage last so as not to precede specializations of
// internalNetMessage

void Server::signalled()
{
  io_.post(boost::bind(&Server::shutdown, this));
}

void Server::shutdown()
{
  message(Verbosity::info, "caught interrupt, shutting down...\n");
  tcp_.stop();
  BOOST_FOREACH(const Connection::Ptr& c, connectionPool_) {
    c->close();
  }
  connectionPool_.clear();
}

void Server::deleteConnection(Connection* connection)
{
  ClientId id = connection->id();
  // TODO: inform of player removal
  players_.get<ClientTag>().erase(id);
  connectionPool_.erase(id);
  message(
      Verbosity::info,
      "removed connection; "+
      boost::lexical_cast<string>(connectionPool_.size())+" remain\n"
    );
}

void Server::sendToAll(const MessageBase& message)
{
  ConnectionPool::index<SequenceTag>::type& connections =
    connectionPool_.get<SequenceTag>();
  for_each(
      connections.begin(), connections.end(),
      boost::bind(&Connection::send, _1, boost::ref(message))
    );
}

#define IGNORE_MESSAGE(type)                               \
template<>                                                 \
void Server::internalNetMessage(                           \
    const Message<MessageType::type>&,                     \
    Connection*                                            \
  )                                                        \
{                                                          \
  message(Verbosity::warning, "ignoring "#type" message"); \
}

IGNORE_MESSAGE(playerAdded)
IGNORE_MESSAGE(updateReadiness)

#undef IGNORE_MESSAGE

template<>
void Server::internalNetMessage(
    const Message<MessageType::addPlayer>& netMessage,
    Connection* connection
  )
{
  PlayerContainer::iterator newIt;
  bool inserted;
  std::tie(newIt, inserted) = players_.insert(
      RemotePlayer(netMessage.payload(), nextPlayerId_++, connection)
    );
  if (inserted) {
    const MessageBase& outMessage =
      Message<MessageType::playerAdded>(newIt->player());
    sendToAll(outMessage);
  } else {
    message(Verbosity::error, "add player failed; id in use");
  }
}

template<>
void Server::internalNetMessage(
    const Message<MessageType::setReadiness>& netMessage,
    Connection* connection
  )
{
  ClientId id = connection->id();
  bool ready = netMessage.payload();
  connection->setReady(ready);
  const MessageBase& outMessage =
    Message<MessageType::updateReadiness>(make_pair(id, ready));
  sendToAll(outMessage);
  typedef ConnectionPool::index<SequenceTag>::type Index;
  Index& conns = connectionPool_.get<SequenceTag>();
  Index::iterator unready =
    find_if(conns.begin(), conns.end(), !boost::bind(&Connection::ready, _1));
  if (unready == conns.end()) {
    // TODO: start game
    throw logic_error("not implemented");
  }
}

void Server::netMessage(
    const MessageBase::Ptr& message,
    Connection* connection
  )
{
  switch (message->type()) {
#define CASE(r, d, value)                                            \
    case MessageType::value:                                         \
      internalNetMessage<MessageType::value>(                        \
          dynamic_cast<const Message<MessageType::value>&>(*message),\
          connection                                                 \
        );                                                           \
      return;
    BOOST_PP_SEQ_FOR_EACH(CASE, _, NIBBLES_MESSAGETYPE_VALUES())
#undef CASE
    default:
      throw logic_error("unknown MessageType");
  }
}

}}

