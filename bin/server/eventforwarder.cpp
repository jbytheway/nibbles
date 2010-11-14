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

void EventForwarder::newNumber(Number const& number)
{
  sink_(Message<MessageType::newNumber>(number));
}

void EventForwarder::tick(Moves const& moves)
{
  sink_(Message<MessageType::tick>(moves));
}

}}

