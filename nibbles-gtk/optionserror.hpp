#ifndef NIBBLES_GTK__OPTIONSERROR_HPP
#define NIBBLES_GTK__OPTIONSERROR_HPP

#include <stdexcept>

namespace nibbles { namespace gtk {

struct OptionsError : public std::runtime_error {
  explicit OptionsError(const std::string& message) : runtime_error(message) {}
};

}}

#endif // NIBBLES_GTK__OPTIONSERROR_HPP

