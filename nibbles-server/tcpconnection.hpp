#ifndef NIBBLES_SERVER__TCPCONNECTION_HPP
#define NIBBLES_SERVER__TCPCONNECTION_HPP

#include <boost/asio.hpp>

#include <nibbles/network.hpp>

#include "connection.hpp"

namespace nibbles { namespace server {

class Server;

class TcpConnection : public Connection {
  public:
    typedef boost::shared_ptr<TcpConnection> Ptr;

    TcpConnection(Server& server);
    virtual ~TcpConnection() {}
    boost::asio::ip::tcp::socket& socket() { return socket_; }
    virtual void start();
  private:
    class TcpReturnPath : public ReturnPath {
      public:
        TcpReturnPath(TcpConnection& connection) : connection_(connection)
        {}
      private:
        TcpConnection& connection_;
    };

    Server& server_;
    boost::asio::ip::tcp::socket socket_;
    static const size_t maxDataLen =
      Network::maxPacketLen+sizeof(Network::PacketLength);
    boost::array<uint8_t, maxDataLen> data_;
    std::size_t dataLen_;
    TcpReturnPath returnPath_;

    void continueRead();
    void handleRead(
        const boost::system::error_code&,
        std::size_t bytes
      );
};

}}

#endif // NIBBLES_SERVER__TCPCONNECTION_HPP

