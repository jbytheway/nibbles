#ifndef NIBBLES_UTILITY__HTON_HPP
#define NIBBLES_UTILITY__HTON_HPP

namespace nibbles { namespace utility {

template<typename T>
inline void hton_impl(T& x) {
  static_assert(sizeof(T) == -1, "type not supported");
};

template<>
inline void hton_impl(uint16_t& x) {
  x = htons(x);
}

template<>
inline void hton_impl(uint32_t& x) {
  x = htonl(x);
}

template<typename T>
inline void hton(T& x) {
  hton_impl<T>(x);
};

}}

#endif // NIBBLES_UTILITY__HTON_HPP

