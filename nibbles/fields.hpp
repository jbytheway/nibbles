#ifndef NIBBLES__FIELDS_HPP
#define NIBBLES__FIELDS_HPP

#include <boost/preprocessor/seq/for_each.hpp>

namespace nibbles {

#define NIBBLES_FIELDS() (blue)(color)(green)(name)(red)

namespace fields {

#define NIBBLES_FIELDS_DECLARE(r, d, field) class field;
BOOST_PP_SEQ_FOR_EACH(NIBBLES_FIELDS_DECLARE, _, NIBBLES_FIELDS())
#undef NIBBLES_FIELDS_DECLARE

}

using namespace fields;

namespace utility {

#define NIBBLES_FIELDS_SPECIALIZE(r, d, field) \
template<>                                     \
struct IsFieldName<field> {                    \
  typedef std::true_type type;                 \
};
BOOST_PP_SEQ_FOR_EACH(NIBBLES_FIELDS_SPECIALIZE, _, NIBBLES_FIELDS())
#undef NIBBLES_FIELDS_SPECIALIZE

#define NIBBLES_FIELDS_SPECIALIZE(r, d, field) \
template<>                                     \
inline const char* getFieldName<field>() {            \
  return #field;                               \
};
BOOST_PP_SEQ_FOR_EACH(NIBBLES_FIELDS_SPECIALIZE, _, NIBBLES_FIELDS())
#undef NIBBLES_FIELDS_SPECIALIZE

}

}

#endif // NIBBLES__FIELDS_HPP

