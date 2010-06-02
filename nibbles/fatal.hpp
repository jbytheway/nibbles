#ifndef NIBBLES__FATAL_HPP
#define NIBBLES__FATAL_HPP

#include <cassert>
#include <ostream>

namespace nibbles {

extern std::ostream& error_stream;

}

#define NIBBLES_FATAL(msg) \
  do { \
    nibbles::error_stream << __FILE__ << ":" << __LINE__ << ":" << \
      __PRETTY_FUNCTION__ <<  ": fatal error: " << msg << std::endl; \
    abort(); \
  } while(false)

#endif // NIBBLES__FATAL_HPP

