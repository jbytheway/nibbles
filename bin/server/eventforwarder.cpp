#include "eventforwarder.hpp"

#include <nibbles/message.hpp>

namespace nibbles { namespace server {

EventForwarder::EventForwarder(boost::function<void (MessageBase const&)> s) :
  sink_(s)
{
}

void EventForwarder::startLevel(LevelDefinition const& levelDef)
{
  sink_(Message<MessageType::levelStart>(levelDef));
}

}}

