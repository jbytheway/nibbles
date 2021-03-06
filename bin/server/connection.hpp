#ifndef NIBBLES_SERVER__CONNECTION_HPP
#define NIBBLES_SERVER__CONNECTION_HPP

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

#include <nibbles/message.hpp>
#include <nibbles/socket.hpp>

namespace nibbles { namespace server {

class Connection : private boost::noncopyable {
  public:
    typedef boost::shared_ptr<Connection> Ptr;

    struct PtrHash {
      size_t operator()(const Ptr& p) const {
        return reinterpret_cast<size_t>(p.get());
      }
    };

    virtual ~Connection() = 0;
    ClientId id() const { return id_; }
    bool ready() const { return ready_; }
    void start() { socket_->read(); }
    void send(const MessageBase& m) { socket_->send(m); }
    void close() { socket_->close(); }
    void setId(ClientId id) { assert(!id_.valid()); id_ = id; }
    void setReady(bool ready) { ready_ = ready; }

    boost::signal<void (MessageBase::Ptr const&, Connection*)> messageSignal;
    boost::signal<void (Connection*)> terminateSignal;
  protected:
    Connection(const Socket::Ptr&);

    ClientId id_;
    Socket::Ptr socket_;
    bool ready_;
};

inline Connection::~Connection() {}

}}

#endif // NIBBLES_SERVER__CONNECTION_HPP

