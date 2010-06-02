#ifndef NIBBLES__UTILITY__ENUM_HPP
#define NIBBLES__UTILITY__ENUM_HPP

#include <stdexcept>
#include <istream>
#include <ostream>

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/serialization/nvp.hpp>

#include <nibbles/fatal.hpp>

// FIXME: the operator>> implementation is hideously slow

#define NIBBLES_UTILITY_ENUM_CASE(r, d, value) \
  case value:                                  \
    return BOOST_PP_STRINGIZE(value);

#define NIBBLES_UTILITY_ENUM_HEADER(name, values)    \
class name {                                         \
  friend class boost::serialization::access;         \
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
          NIBBLES_FATAL("invalid enumeration value"); \
      }                                              \
    }                                                \
                                                     \
    friend bool operator<=(name l, name r) {         \
      return l.value <= r.value;                     \
    }                                                \
  private:                                           \
    internal_enum value;                             \
                                                     \
    template<class Archive>                          \
    void serialize(Archive& ar, unsigned int const /*version*/) \
    {                                                \
      ar & BOOST_SERIALIZATION_NVP(value);           \
    }                                                \
};                                                   \
                                                     \
inline std::istream& operator>>(std::istream& is, name& v) { \
  std::string s;                                     \
  is >> s;                                           \
  for (int i=0; i<name::max; ++i) {                  \
    if (name(i).string() == s) {                     \
      v = name(i);                                   \
    }                                                \
  }                                                  \
  return is;                                         \
}                                                    \
                                                     \
inline std::ostream& operator<<(std::ostream& os, const name v) { \
  return os << v.string();                           \
}

#endif // NIBBLES__UTILITY__ENUM_HPP

