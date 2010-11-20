#ifndef NIBBLES__FIELDS_HPP
#define NIBBLES__FIELDS_HPP

#include <boost/preprocessor/seq/for_each.hpp>

#include <nibbles/utility/isfield.hpp>

namespace nibbles {

#define NIBBLES_FIELDS() \
  (blocks)(blue)(board)(clientId)(color)(controls)(deathScore)\
  (definition)(direction)(green)(growthFactor)(h)(id)(level)\
  (levelId)(levels)(max)(min)(name)(number)(pendingGrowth)\
  (player)(players)(point)(points)(position)(random)\
  (ready)(red)(score)(settings)(snakes)(startLength)(startLives)(starts)\
  (states)(tickInterval)(value)(w)(x)(y)

namespace fields {

#define NIBBLES_FIELDS_DECLARE(r, d, field) class field;
BOOST_PP_SEQ_FOR_EACH(NIBBLES_FIELDS_DECLARE, _, NIBBLES_FIELDS())
#undef NIBBLES_FIELDS_DECLARE

}

using namespace fields;

namespace utility {

#define NIBBLES_FIELDS_SPECIALIZE(r, d, field) \
template<>                                     \
struct IsField<field> {                        \
  typedef std::true_type type;                 \
};
BOOST_PP_SEQ_FOR_EACH(NIBBLES_FIELDS_SPECIALIZE, _, NIBBLES_FIELDS())
#undef NIBBLES_FIELDS_SPECIALIZE

#define NIBBLES_FIELDS_SPECIALIZE(r, d, field) \
template<>                                     \
inline const char* getFieldName<field>() {     \
  return BOOST_PP_STRINGIZE(field);            \
};
BOOST_PP_SEQ_FOR_EACH(NIBBLES_FIELDS_SPECIALIZE, _, NIBBLES_FIELDS())
#undef NIBBLES_FIELDS_SPECIALIZE

}

}

#endif // NIBBLES__FIELDS_HPP

