#ifndef NIBBLES__UTILITY__ARRAYSERIALIZATION_HPP
#define NIBBLES__UTILITY__ARRAYSERIALIZATION_HPP

#include <array>

namespace boost { namespace serialization {

// This breaks the Golden Rule of Template Specialization, and so will break
// when boost::serialization catches up to C++0x, but never mind...
template<class Archive, class U, std::size_t N>
inline void serialize(
    Archive& ar,
    std::array<U, N>& array,
    const unsigned int /*file_version*/
  )
{
  ar & make_array(array.data(), N);
}

}}

#endif // NIBBLES__UTILITY__ARRAYSERIALIZATION_HPP

