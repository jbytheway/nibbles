#ifndef NIBBLES__UTILITY__SLICE_HPP
#define NIBBLES__UTILITY__SLICE_HPP

#include <boost/fusion/include/iterator_range.hpp>
#include <boost/fusion/include/as_vector.hpp>

namespace nibbles { namespace utility {

namespace mpl = boost::mpl;
namespace fusion = boost::fusion;

namespace detail {

template<typename... Args>
struct VariadicSequence;

template<>
struct VariadicSequence<>
{
  typedef mpl::vector<>::type type;
};

template<typename First, typename... More>
struct VariadicSequence<First, More...>
{
  typedef typename mpl::push_front<
      typename VariadicSequence<More...>::type,
      First
    >::type type;
};

template<typename... Args>
struct VariadicVector {
  typedef typename fusion::result_of::as_vector<
      typename VariadicSequence<Args...>::type
    >::type type;
};

}

template<size_t begin, size_t end, typename... Args>
struct SliceResult {
  static const size_t actualEnd =
    ( end == size_t(-1) ? sizeof...(Args) : end );
  static_assert(begin <= actualEnd, "slice ends before it begins");

  // Failure here means that Args was too short to be so sliced
  BOOST_MPL_ASSERT_RELATION(actualEnd,<=,sizeof...(Args));

  typedef typename detail::VariadicVector<Args...>::type FullVector;
  typedef typename fusion::result_of::begin<FullVector>::type VectorBegin;
  typedef typename fusion::result_of::advance_c<
      VectorBegin, begin
    >::type Begin;
  typedef typename fusion::result_of::advance_c<
      VectorBegin, actualEnd
    >::type End;
  typedef typename fusion::iterator_range<Begin, End> Range;
  typedef typename fusion::result_of::as_vector<Range>::type type;
};

template<size_t begin, size_t end = -1, typename... Args>
inline typename SliceResult<begin, end, Args...>::type slice(Args&&... args)
{
  // TODO: possibly inefficient; check comiler cleverness if worried
  typedef SliceResult<begin, end, Args...> Result;
  typename Result::FullVector v(args...);
  typename Result::Begin start(v);
  typename Result::End finish(v);
  typename Result::Range r(start, finish);
  return typename Result::type(r);
}

}}

#endif // NIBBLES__UTILITY__SLICE_HPP

