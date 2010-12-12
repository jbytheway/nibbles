#ifndef NIBBLES_UTILITY__NTOH_HPP
#define NIBBLES_UTILITY__NTOH_HPP

namespace nibbles { namespace utility {

template<typename T>
inline void ntoh_impl(T& x) {
  static_assert(sizeof(T) == -1, "type not supported");
};

template<>
inline void ntoh_impl(uint16_t& x) {
  x = ntohs(x);
}

template<>
inline void ntoh_impl(uint32_t& x) {
  x = ntohl(x);
}

template<typename T>
inline void ntoh(T& x) {
  ntoh_impl<T>(x);
};

}}

#endif // NIBBLES_UTILITY__NTOH_HPP

