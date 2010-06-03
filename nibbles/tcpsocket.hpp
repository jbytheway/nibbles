#ifndef NIBBLES__TCPSOCKET_HPP
#define NIBBLES__TCPSOCKET_HPP

#include <string>

#include <boost/asio.hpp>

#include <nibbles/socket.hpp>
#include <nibbles/network.hpp>
#include <nibbles/utility/messagehandler.hpp>

namespace nibbles {

class TcpSocket : public Socket {
  public:
    typedef boost::shared_ptr<TcpSocket> Ptr;

    template<typename... Args>
    static Ptr create(Args&&... args)
    {
      Ptr p(new TcpSocket(std::forward<Args>(args)...));
      p->ptrToThis_ = p;
      return p;
    }

    virtual void connect();
    virtual void read() { continueRead(); }
    virtual void send(const MessageBase&);
    virtual void close() { socket_.close(); }

    boost::asio::ip::tcp::socket& socket() { return socket_; }
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
    boost::weak_ptr<TcpSocket> ptrToThis_;

    static const size_t maxDataLen =
      Network::maxPacketLen+sizeof(Network::PacketLength);
    boost::array<uint8_t, maxDataLen> data_; // incoming data
    std::size_t dataLen_;

    void continueRead();
    void handleRead(
        const boost::system::error_code&,
        std::size_t bytes,
        const Ptr&
      );

    // deque maybe more efficient but wouldn't play nice with asio.
    // Can't use an array because may be multiple packets outgoing.
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

