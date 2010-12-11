#ifndef NIBBLES_UTILITY__DATACLASSCOMPARE_HPP
#define NIBBLES_UTILITY__DATACLASSCOMPARE_HPP

#include <boost/fusion/include/less.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/as_vector.hpp>

namespace nibbles { namespace utility {

template<typename Class>
class DataClassCompare {
  public:
    struct ProjectSecond {
      template<typename Sig>
      struct result;

      template<typename Pair>
      struct result<ProjectSecond(Pair)> {
        typedef typename std::remove_reference<Pair>::type P;
        typedef typename P::second_type const& type;
      };

      template<typename Pair>
      typename Pair::second_type const& operator()(Pair const& p) const {
        return p.second;
      }
    };
    inline bool operator()(Class const& l, Class const& r) const {
      ProjectSecond p;
      using namespace boost::fusion;
      auto lv = transform(l.fields_, p);
      auto rv = transform(r.fields_, p);
      return lv < rv;
    }
};

}}

#endif // NIBBLES_UTILITY__DATACLASSCOMPARE_HPP

