#ifndef NIBBLES__UTILITY__NULLDELETER_HPP
#define NIBBLES__UTILITY__NULLDELETER_HPP

namespace nibbles { namespace utility {

struct NullDeleter {
  void operator()(void const*) const {
  }
};

}}

#endif // NIBBLES__UTILITY__NULLDELETER_HPP

