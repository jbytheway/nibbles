#ifndef NIBBLES_CLIENT__TCPCLIENT_HPP
#define NIBBLES_CLIENT__TCPCLIENT_HPP

#include <nibbles/client/client.hpp>

namespace nibbles { namespace client {

class TcpClient : public Client {
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
    utility::MessageHandler& out_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::socket socket_;

    // deque maybe more efficient but wouldn't play nice with asio
    std::string outgoing_; // waiting to be written
    std::string writing_; // being written right now

    virtual void send(const MessageBase&);
    void startWrite();
    void handleWrite(
        const boost::system::error_code&,
        const size_t bytes_transferred
      );
};

}}

#endif // NIBBLES_CLIENT__TCPCLIENT_HPP

