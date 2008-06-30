#ifndef NIBBLES__UTILITY__GETFIELDNAME_HPP
#define NIBBLES__UTILITY__GETFIELDNAME_HPP

namespace nibbles { namespace utility {

template<typename Field>
inline const char* getFieldName()
{
  static_assert(
      IsFieldName<Field>::type::value, "template parameter must be a field"
    );
  return NULL;
}

}}

#endif // NIBBLES__UTILITY__GETFIELDNAME_HPP

