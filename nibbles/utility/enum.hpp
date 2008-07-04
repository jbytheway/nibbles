#ifndef NIBBLES__UTILITY__ENUM_HPP
#define NIBBLES__UTILITY__ENUM_HPP

#include <stdexcept>

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>

#define NIBBLES_UTILITY_ENUM_CASE(r, d, value) \
  case value:                                  \
    return BOOST_PP_STRINGIZE(value);

#define NIBBLES_UTILITY_ENUM_HEADER(name, values)    \
class name {                                         \
  public:                                            \
    enum internal_enum {                             \
      BOOST_PP_SEQ_ENUM(values),                     \
      max                                            \
    };                                               \
                                                     \
    name() {};                                       \
    explicit name(int const v) : value(internal_enum(v)) {} \
    name(internal_enum const v) : value(v) {}        \
    operator internal_enum() const { return value; } \
                                                     \
    std::string string() const {                     \
      switch (value) {                               \
        BOOST_PP_SEQ_FOR_EACH(NIBBLES_UTILITY_ENUM_CASE, _, values) \
        default:                                     \
          throw std::logic_error("invalid enumeration value"); \
      }                                              \
    }                                                \
                                                     \
    friend bool operator<=(name l, name r) {         \
      return l.value <= r.value;                     \
    }                                                \
  private:                                           \
    internal_enum value;                             \
};

#endif // NIBBLES__UTILITY__ENUM_HPP

