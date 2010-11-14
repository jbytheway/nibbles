#ifndef NIBBLES_SERVER__EVENTFORWARDER_HPP
#define NIBBLES_SERVER__EVENTFORWARDER_HPP

#include <boost/function.hpp>

#include <nibbles/gameeventhandler.hpp>
#include <nibbles/messagebase.hpp>

namespace nibbles { namespace server {

class EventForwarder : public GameEventHandler {
  public:
    EventForwarder(boost::function<void (MessageBase const&)>);

    virtual void startLevel(LevelDefinition const&);
    virtual void newNumber(Number const&);
  private:
    boost::function<void (MessageBase const&)> sink_;
};

}}

#endif // NIBBLES_SERVER__EVENTFORWARDER_HPP

