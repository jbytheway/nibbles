#ifndef NIBBLES_SERVER__SERVER_HPP
#define NIBBLES_SERVER__SERVER_HPP

#include <unordered_set>
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include "options.hpp"
#include "tcpserver.hpp"
#include "remoteplayer.hpp"

namespace nibbles { namespace server {

class Server : boost::noncopyable
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

    typedef std::unordered_set<Connection::Ptr, Connection::PtrHash>
      ConnectionPool;
    ConnectionPool connectionPool_;

    struct ConnectionTag;

    typedef boost::multi_index_container<
        RemotePlayer,
        boost::multi_index::indexed_by<
          boost::multi_index::hashed_unique<
            BOOST_MULTI_INDEX_CONST_MEM_FUN(RemotePlayer, PlayerId, id)
          >,
          boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<ConnectionTag>,
            BOOST_MULTI_INDEX_CONST_MEM_FUN(
                RemotePlayer, Connection::Ptr, connection
              )
          >
        >
      > PlayerContainer;
    PlayerContainer players_;
    PlayerId nextPlayerId_;

    void netMessage(const MessageBase&, Connection*);
    void signalled();
    void shutdown();
    void deleteConnection(Connection* connection);

    template<int Type>
    void internalNetMessage(const Message<Type>&, Connection*);
};

}}

#endif // NIBBLES_SERVER__SERVER_HPP

