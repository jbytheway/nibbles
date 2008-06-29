#ifndef NIBBLES__UTILITY__ISFIELDNAME_HPP
#define NIBBLES__UTILITY__ISFIELDNAME_HPP

namespace nibbles { namespace utility {

template<typename T>
struct IsFieldName {
  typedef std::false_type type;
};

}}

#endif // NIBBLES__UTILITY__ISFIELDNAME_HPP

