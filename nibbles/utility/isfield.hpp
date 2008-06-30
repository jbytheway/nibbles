#ifndef NIBBLES__UTILITY__ISFIELD_HPP
#define NIBBLES__UTILITY__ISFIELD_HPP

namespace nibbles { namespace utility {

template<typename T>
struct IsField {
  typedef std::false_type type;
};

}}

#endif // NIBBLES__UTILITY__ISFIELD_HPP

