#ifndef NIBBLES__TCPSOCKET_HPP
#define NIBBLES__TCPSOCKET_HPP

#include <string>

#include <boost/asio.hpp>

#include <nibbles/socket.hpp>
#include <nibbles/utility/messagehandler.hpp>

namespace nibbles {

class TcpSocket : public virtual Socket {
  public:
    virtual void send(const MessageBase&);
  protected:
    TcpSocket(
        boost::asio::io_service&,
        utility::MessageHandler& out
      );
    TcpSocket(
        boost::asio::io_service&,
        utility::MessageHandler& out,
        std::string const& address,
        uint16_t const port
      );
    utility::MessageHandler& out_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::socket socket_;

    // deque maybe more efficient but wouldn't play nice with asio
    std::string outgoing_; // waiting to be written
    std::string writing_; // being written right now

    void startWrite();
    void handleWrite(
        const boost::system::error_code&,
        const size_t bytes_transferred
      );
};

}

#endif // NIBBLES__TCPSOCKET_HPP

