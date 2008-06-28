#ifndef NIBBLES__UTILITY__MESSAGEHANDLER_HPP
#define NIBBLES__UTILITY__MESSAGEHANDLER_HPP

#include <nibbles/utility/verbosity.hpp>

namespace nibbles { namespace utility {

class MessageHandler {
  public:
    virtual void message(Verbosity, const std::string& message) = 0;
  protected:
    MessageHandler() {}
    MessageHandler(const MessageHandler&) {}
};

}}

#endif // NIBBLES__UTILITY__MESSAGEHANDLER_HPP

