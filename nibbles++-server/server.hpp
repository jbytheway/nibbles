#ifndef NIBBLES_SERVER__SERVER_HPP
#define NIBBLES_SERVER__SERVER_HPP

#include <boost/utility.hpp>
#include <boost/asio.hpp>

#include "options.hpp"

namespace nibbles { namespace server {

class Server : boost::noncopyable
{
  public:
    Server(boost::asio::io_service& io, std::ostream& out, const Options& o);
    void serve();
  private:
    boost::asio::io_service& io;
    std::ostream& out;
    const Options options;
    boost::asio::ip::tcp::tcp::acceptor tcpAcceptor;

    void writeLine(Verbosity, const std::string&);
};

}}

#endif // NIBBLES_SERVER__SERVER_HPP

