#ifndef NIBBLES__UTILITY__DATACLASS_HPP
#define NIBBLES__UTILITY__DATACLASS_HPP

#include <boost/fusion/algorithm/transformation.hpp>
#include <boost/fusion/algorithm/iteration.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/serialization/access.hpp>

#include <nibbles/utility/isfieldname.hpp>

namespace nibbles { namespace utility {

// Helper metafunction to construct the field map out of the list of
// type/names given; basically this means taking them in pairs
template<typename... Fields>
struct FieldMapHelper;

template<>
struct FieldMapHelper<> {
  typedef boost::fusion::map<> type;
};

template<typename Leftover>
struct FieldMapHelper<Leftover> {
  typedef void type;
};

template<typename FieldType, typename FieldName, typename... More>
struct FieldMapHelper<FieldType, FieldName, More...>
{
  static_assert(
      IsFieldName<FieldName>::type::value,
      "argument to DataClass was not a field name when it should have been"
    );
  typedef typename FieldMapHelper<More...>::type MapSoFar;
  typedef typename boost::fusion::result_of::push_back<
      MapSoFar,
      boost::fusion::pair<FieldName, FieldType>
    >::type type;
};

template<typename... Fields>
class DataClass {
  friend class boost::serialization::access;
  private:
    static_assert(!(sizeof...(Fields)%2), "odd number of template args given");
    typedef typename FieldMapHelper<Fields...>::type FieldMap;
    FieldMap fields_;

    template<class Archive>
    struct Archiver {
      Archiver(Archive& archive) : archive_(archive) {}
      Archive& archive_;
      template<typename Pair>
      void operator()(Pair& pair) {
        archive_ & pair.second;
      }
    };

    template<class Archive>
    void serialize(Archive& ar, unsigned int const version)
    {
      boost::fusion::for_each(fields_, Archiver<Archive>(ar));
    }
};

}}

#endif // NIBBLES__UTILITY__DATACLASS_HPP

