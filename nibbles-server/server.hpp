#ifndef NIBBLES_SERVER__SERVER_HPP
#define NIBBLES_SERVER__SERVER_HPP

#include <unordered_set>
#include <boost/utility.hpp>
#include <boost/asio.hpp>

#include "options.hpp"
#include "tcpserver.hpp"

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

    void packet(const Packet&, const ReturnPath&);
    void shutdown();
    void deleteConnection(Connection* connection);
};

}}

#endif // NIBBLES_SERVER__SERVER_HPP

