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

}

#endif // NIBBLES__FIELDS_HPP

