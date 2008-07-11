#ifndef NIBBLES_SERVER__CONNECTION_HPP
#define NIBBLES_SERVER__CONNECTION_HPP

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

#include <nibbles/message.hpp>

namespace nibbles { namespace server {

class Connection : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Connection> Ptr;

    struct PtrHash {
      size_t operator()(const Ptr& p) const {
        return reinterpret_cast<size_t>(p.get());
      }
    };

    virtual ~Connection() = 0;
    virtual void start() = 0;
    virtual void close() = 0;

    boost::signal<void (MessageBase const&, Connection*)> messageSignal;
    boost::signal<void (Connection*)> terminateSignal;
};

inline Connection::~Connection() {}

}}

#endif // NIBBLES_SERVER__CONNECTION_HPP

