#ifndef NIBBLES__UTILITY__ENUM_HPP
#define NIBBLES__UTILITY__ENUM_HPP

#include <boost/preprocessor/seq/enum.hpp>

#define NIBBLES_UTILITY_ENUM_HEADER(name, values)    \
class name {                                         \
  public:                                            \
    enum internal_enum {                             \
      BOOST_PP_SEQ_ENUM(values)                      \
    };                                               \
                                                     \
    name() {};                                       \
    name(internal_enum const v) : value(v) {}        \
    operator internal_enum() const { return value; } \
    friend bool operator<=(name l, name r) {         \
      return l.value <= r.value;                     \
    }                                                \
  private:                                           \
    internal_enum value;                             \
};

#endif // NIBBLES__UTILITY__ENUM_HPP

