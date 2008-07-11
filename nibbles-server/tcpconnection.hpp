#ifndef NIBBLES_SERVER__TCPCONNECTION_HPP
#define NIBBLES_SERVER__TCPCONNECTION_HPP

#include <boost/weak_ptr.hpp>
#include <boost/asio.hpp>

#include <nibbles/network.hpp>

#include "connection.hpp"

namespace nibbles { namespace server {

class Server;

class TcpConnection : public Connection {
  public:
    typedef boost::shared_ptr<TcpConnection> Ptr;

    static Ptr create(Server& server)
    {
      Ptr p(new TcpConnection(server));
      p->ptrToThis_ = p;
      return p;
    }

    virtual ~TcpConnection() {}
    boost::asio::ip::tcp::socket& socket() { return socket_; }
    virtual void start();
    virtual void close();
  private:
    TcpConnection(Server& server);
    
    Server& server_;
    boost::weak_ptr<TcpConnection> ptrToThis_;
    boost::asio::ip::tcp::socket socket_;
    static const size_t maxDataLen =
      Network::maxPacketLen+sizeof(Network::PacketLength);
    boost::array<uint8_t, maxDataLen> data_;
    std::size_t dataLen_;

    void continueRead();
    void handleRead(
        const boost::system::error_code&,
        std::size_t bytes,
        const Ptr&
      );
};

}}

#endif // NIBBLES_SERVER__TCPCONNECTION_HPP

