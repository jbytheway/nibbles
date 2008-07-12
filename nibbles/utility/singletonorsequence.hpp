#ifndef NIBBLES__UTILITY__SINGLETONORSEQUENCE_HPP
#define NIBBLES__UTILITY__SINGLETONORSEQUENCE_HPP

#include <boost/fusion/include/front.hpp>

namespace nibbles { namespace utility {

namespace mpl = boost::mpl;
namespace fusion = boost::fusion;

namespace detail {

template<typename T>
struct IsSingleton :
  mpl::and_<
    typename fusion::traits::is_sequence<T>::type,
    typename mpl::equal_to<
      typename fusion::result_of::size<T>::type,
      mpl::size_t<1>
    >::type
  >
{};

}

// If argument is a singleton sequence, returns its element, otherwise returns
// the whole argument

template<typename T>
inline typename boost::disable_if<detail::IsSingleton<T>, T>::type
singletonOrSequence(T&& t) {
  return t;
}

template<typename T>
inline typename boost::enable_if<
    detail::IsSingleton<T>,
    typename fusion::result_of::front<T>::type
  >::type
singletonOrSequence(T&& t) {
  return fusion::front(t);
}

}}

#endif // NIBBLES__UTILITY__SINGLETONORSEQUENCE_HPP

