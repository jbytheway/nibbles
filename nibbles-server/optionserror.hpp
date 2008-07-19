#ifndef NIBBLES_SERVER__OPTIONSERROR_HPP
#define NIBBLES_SERVER__OPTIONSERROR_HPP

#include <stdexcept>

namespace nibbles { namespace server {

struct OptionsError : public std::runtime_error {
  explicit OptionsError(const std::string& message) : runtime_error(message) {}
};

}}

#endif // NIBBLES_SERVER__OPTIONSERROR_HPP

