#ifndef NIBBLES_CLIENT__TCPCLIENT_HPP
#define NIBBLES_CLIENT__TCPCLIENT_HPP

#include <nibbles/client/client.hpp>

namespace nibbles { namespace client {

class TcpClient : public Client {
  public:
    TcpClient(
        boost::asio::io_service&,
        std::string const& address,
        uint16_t const port
      );
    virtual ~TcpClient() {}
    virtual void connect();
  private:
    boost::asio::io_service& io_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::socket socket_;
};

}}

#endif // NIBBLES_CLIENT__TCPCLIENT_HPP

