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
  tcp_(*this),
  game_(o.gameSettings()),
  gameTickTimer_(io),
  pausing_(false),
  paused_(false),
  forwarder_(boost::bind(&Server::sendToAll, this, _1))
{
  signalCatcher.connect(boost::bind(&Server::signalled, this));

  boost::filesystem::path levelsFile(o.levelPack);
  if (levelsFile.empty()) {
    throw runtime_error("no levelpack specified");
  }
  boost::filesystem::ifstream ifs(levelsFile);
  if (!ifs.is_open()) {
    throw runtime_error("failed to open levelpack "+levelsFile.file_string());
  }
  boost::archive::xml_iarchive ia(ifs);
  ia >> BOOST_SERIALIZATION_NVP(levelPack_);
  game_.get<levels>() = levelPack_;
}

void Server::serve()
{
  if (options_.useTcp) {
    ip::address addr(ip::address::from_string(options_.tcpAddress));
    tcp_.serve(ip::tcp::endpoint(addr, options_.tcpPort));
  }

  io_.run(); // Blocks until all stuff is done
  message(Verbosity::info, "server done");
}

void Server::addConnection(const Connection::Ptr& connection)
{
  if (game_.started()) {
    message(Verbosity::error, "connection failed; game started");
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
        boost::lexical_cast<string>(connectionPool_.size())+" now exist"
      );
    sendStateToConnection(connection);
  } else {
    message(Verbosity::error, "connection failed; id in use");
    connection->close();
  }
}

void Server::message(Verbosity v, const string& message)
{
  if (options_.verbosity <= v)
    out_ << message << std::endl;
}

// Server::netMessage last so as not to precede specializations of
// internalNetMessage

void Server::setReadiness(Connection* connection, bool ready)
{
  ClientId id = connection->id();
  connection->setReady(ready);
  const MessageBase& outMessage =
    Message<MessageType::updateReadiness>(make_pair(id, ready));
  sendToAll(outMessage);
}

void Server::signalled()
{
  io_.post(boost::bind(&Server::shutdown, this));
}

void Server::shutdown()
{
  message(Verbosity::info, "caught interrupt, shutting down...");
  tcp_.stop();
  gameTickTimer_.cancel();
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
      boost::lexical_cast<string>(connectionPool_.size())+" remain"
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

void Server::sendStateToConnection(Connection::Ptr const& connection)
{
  BOOST_FOREACH(Connection::Ptr const& conn, connectionPool_) {
    connection->send(Message<MessageType::updateReadiness>(
        {conn->id(), conn->ready()}
    ));
  }

  BOOST_FOREACH(RemotePlayer const& player, players_) {
    connection->send(Message<MessageType::playerAdded>(player));
  }
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
IGNORE_MESSAGE(gameStart)
IGNORE_MESSAGE(levelStart)
IGNORE_MESSAGE(newNumber)
IGNORE_MESSAGE(tick)
IGNORE_MESSAGE(gameOver)

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
      Message<MessageType::playerAdded>(*newIt);
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
  setReadiness(connection, netMessage.payload());
  checkForGameStart();
}

template<>
void Server::internalNetMessage(
    const Message<MessageType::command>& netMessage,
    Connection* connection
  )
{
  ClientId clientId = connection->id();
  PlayerId playerId = netMessage.payload().first;

  auto it = players_.find(playerId);
  if (it == players_.end()) {
    message(Verbosity::warning, "turn for non-existant player");
    return;
  }
  ClientId purportedClientId = it->clientId();
  if (clientId != purportedClientId) {
    message(Verbosity::warning, "attempted spoofed command");
    return;
  }
  Command command = netMessage.payload().second;
  switch (command) {
    case Direction::up:
    case Direction::down:
    case Direction::left:
    case Direction::right:
      it->queueTurn(static_cast<Direction>(command));
      break;
    case Command::pause:
      togglePaused();
      break;
    default:
      NIBBLES_FATAL("unexpected command");
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
      NIBBLES_FATAL("unknown MessageType");
  }
}

void Server::checkForGameStart()
{
  if (connectionPool_.empty() || players_.empty())
    return;
  typedef ConnectionPool::index<SequenceTag>::type Index;
  Index& conns = connectionPool_.get<SequenceTag>();
  Index::iterator unready =
    find_if(conns.begin(), conns.end(), !boost::bind(&Connection::ready, _1));
  if (unready != conns.end()) {
    return;
  }
  auto const& settings = game_.get<fields::settings>();
  sendToAll(Message<MessageType::gameStart>(settings));
  BOOST_FOREACH(auto const& player, players_.get<SequenceTag>()) {
    scorer_.add(player.id(), settings.get<startLives>());
    player.reset();
  }
  game_.start(players_.get<SequenceTag>(), forwarder_);
  tick();
}

void Server::tick(const boost::system::error_code& e)
{
  if (e) {
    message(Verbosity::info, "game interrupted");
    return;
  }
  if (pausing_) {
    pausing_ = false;
    paused_ = true;
    return;
  }
  auto interval = game_.get<tickInterval>();
  gameTickTimer_.expires_from_now(interval);

  Moves moves;
  BOOST_FOREACH(auto const& player, players_) {
    auto move = player.dequeue();
    if (move) {
      moves.push_back({player.id(), *move});
    }
  }
  forwarder_.tick(moves);
  auto result = game_.tick(scorer_, forwarder_, moves);
  if (result == TickResult::gameOver) {
    gameEnd();
    return;
  } else if (result >= TickResult::advanceLevel) {
    // This means the level is reset, so we need to flush the state in the
    // players
    BOOST_FOREACH(auto const& player, players_) {
      player.reset();
    }
  }
  gameTickTimer_.async_wait(boost::bind(
        &Server::tick, this, boost::asio::placeholders::error
      ));
}

void Server::togglePaused()
{
  if (paused_) {
    paused_ = false;
    tick();
  } else {
    pausing_ = true;
  }
}

void Server::gameEnd()
{
  // TODO: high score info
  sendToAll(Message<MessageType::gameOver>(0));
  BOOST_FOREACH(auto& connection, connectionPool_.get<SequenceTag>()) {
    setReadiness(&*connection, false);
  }
  scorer_.clear();
}

}}

