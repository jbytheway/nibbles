#ifndef NIBBLES_CLIENT__TCPCLIENT_HPP
#define NIBBLES_CLIENT__TCPCLIENT_HPP

#include <nibbles/tcpsocket.hpp>
#include <nibbles/client/client.hpp>

namespace nibbles { namespace client {

class TcpClient : public Client, public TcpSocket {
  public:
    TcpClient(
        boost::asio::io_service&,
        utility::MessageHandler& out,
        std::string const& address,
        uint16_t const port
      );
    virtual ~TcpClient() {}
    virtual void connect();
  private:
};

}}

#endif // NIBBLES_CLIENT__TCPCLIENT_HPP

