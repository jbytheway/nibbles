#ifndef NIBBLES__UTILITY__DATACLASS_HPP
#define NIBBLES__UTILITY__DATACLASS_HPP

#include <boost/fusion/algorithm/transformation.hpp>
#include <boost/fusion/algorithm/iteration.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/list.hpp>
#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/call_traits.hpp>

// Include the serialization implementations for STL stuff needed here so that
// it's not necessary to remember to use it every time elsewhere
#include <boost/serialization/vector.hpp>

#include <nibbles/utility/isfield.hpp>
#include <nibbles/utility/getfieldname.hpp>

namespace nibbles { namespace utility {

// Helper metafunction to construct the field map out of the list of
// type/names given; basically this means taking them in pairs
// See below for definition
template<typename... Fields>
struct FieldMapHelper;

template<typename Derived, typename... Fields>
class DataClass {
  private:
    static_assert(
        !(sizeof...(Fields)%2),
        "odd number of field template args given"
      );
    typedef typename FieldMapHelper<Fields...>::type FieldMapSequence;
    typedef typename boost::fusion::result_of::as_map<FieldMapSequence>::type FieldMap;
    FieldMap fields_;
  public:
    DataClass()
    {
    }
    
    DataClass(Derived& copy) :
      fields_(copy.fields_)
    {
    }

    DataClass(const Derived& copy) :
      fields_(copy.fields_)
    {
    }

    template<typename... Args>
    DataClass(Args&&... args) :
      fields_(boost::fusion::make_vector(args...))
    {
    }

    typedef DataClass base;

    template<typename Field>
    typename boost::call_traits<
        typename boost::fusion::result_of::at_key<FieldMap, Field>::type
      >::reference get() {
      return boost::fusion::at_key<Field>(fields_);
    }

    template<typename Field>
    typename boost::call_traits<
        typename boost::fusion::result_of::at_key<FieldMap, Field>::type
      >::const_reference get() const {
      return boost::fusion::at_key<Field>(fields_);
    }
  private:
    template<class Archive>
    struct Archiver {
      Archiver(Archive& archive) : archive_(archive) {}
      Archive& archive_;
      template<typename Pair>
      void operator()(Pair& pair) const {
        archive_ & boost::serialization::make_nvp(
            utility::getFieldName<typename Pair::first_type>(), pair.second
          );
      }
    };
  public:
    // Don't really want this method to be public, but a friend declaration for
    // boost::serialization::access doesn't get inherited by derived classes,
    // so we'll live with it for now
    template<class Archive>
    void serialize(Archive& ar, unsigned int const /*version*/)
    {
      boost::fusion::for_each(fields_, Archiver<Archive>(ar));
    }
};

template<>
struct FieldMapHelper<> {
  typedef boost::fusion::list<> type;
};

template<typename Leftover>
struct FieldMapHelper<Leftover> {
  typedef void type;
};

template<typename FieldType, typename FieldName, typename... More>
struct FieldMapHelper<FieldType, FieldName, More...>
{
  static_assert(
      IsField<FieldName>::type::value,
      "argument to DataClass was not a field name when it should have been"
    );
  typedef typename FieldMapHelper<More...>::type MapSoFar;
  typedef typename boost::fusion::result_of::push_front<
      MapSoFar,
      boost::fusion::pair<FieldName, FieldType>
    >::type type;
};

}}

#endif // NIBBLES__UTILITY__DATACLASS_HPP
