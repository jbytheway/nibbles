#ifndef NIBBLES_SERVER__SERVER_HPP
#define NIBBLES_SERVER__SERVER_HPP

#include <unordered_set>
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <nibbles/game.hpp>
#include <nibbles/scoretracker.hpp>
#include <nibbles/utility/messagehandler.hpp>

#include "options.hpp"
#include "tcpserver.hpp"
#include "remoteplayer.hpp"
#include "eventforwarder.hpp"

namespace nibbles { namespace server {

class Server : public utility::MessageHandler, private boost::noncopyable
{
  public:
    Server(boost::asio::io_service& io, std::ostream& out, const Options& o);
    void serve();
    boost::asio::io_service& io() { return io_; }
    void addConnection(const Connection::Ptr&);
    void message(utility::Verbosity, const std::string&);
  private:
    boost::asio::io_service& io_;
    std::ostream& out_;
    const Options options_;
    TcpServer tcp_;

    LevelPack levelPack_;

    struct SequenceTag;
    typedef boost::multi_index_container<
        Connection::Ptr,
        boost::multi_index::indexed_by<
          boost::multi_index::hashed_unique<
            BOOST_MULTI_INDEX_CONST_MEM_FUN(Connection, ClientId, id)
          >,
          boost::multi_index::sequenced<
            boost::multi_index::tag<SequenceTag>
          >
        >
      > ConnectionPool;
    ConnectionPool connectionPool_;
    ClientId nextClientId_;

    struct ClientTag;
    typedef boost::multi_index_container<
        RemotePlayer,
        boost::multi_index::indexed_by<
          boost::multi_index::hashed_unique<
            BOOST_MULTI_INDEX_CONST_MEM_FUN(RemotePlayer, PlayerId, id)
          >,
          boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<ClientTag>,
            BOOST_MULTI_INDEX_CONST_MEM_FUN(
                RemotePlayer, ClientId, clientId
              )
          >,
          boost::multi_index::sequenced<
            boost::multi_index::tag<SequenceTag>
          >
        >
      > PlayerContainer;
    PlayerContainer players_;
    PlayerId nextPlayerId_;

    Game game_;
    boost::asio::deadline_timer gameTickTimer_;
    ScoreTracker scorer_;
    EventForwarder forwarder_;

    void netMessage(const MessageBase::Ptr&, Connection*);
    void signalled();
    void shutdown();
    void deleteConnection(Connection* connection);
    void sendToAll(const MessageBase&);
    void sendStateToConnection(Connection::Ptr const&);

    template<int Type>
    void internalNetMessage(const Message<Type>&, Connection*);

    void checkForGameStart();
    void tick(const boost::system::error_code& e = boost::system::error_code());
};

}}

#endif // NIBBLES_SERVER__SERVER_HPP

