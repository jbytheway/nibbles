#ifndef NIBBLES__UTILITY__DATACLASS_HPP
#define NIBBLES__UTILITY__DATACLASS_HPP

#include <boost/mpl/vector.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/fusion/algorithm/transformation.hpp>
#include <boost/fusion/algorithm/iteration.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/list.hpp>
#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/serialization/access.hpp>
#include <boost/call_traits.hpp>

// Include the serialization implementations for STL stuff needed here so that
// it's not necessary to remember to use it every time elsewhere
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <nibbles/utility/isfield.hpp>
#include <nibbles/utility/getfieldname.hpp>
#include <nibbles/utility/singletonorsequence.hpp>
#include <nibbles/utility/slice.hpp>
#include <nibbles/utility/arrayserialization.hpp>

#define NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(type)           \
  template<typename... Args>                                  \
  explicit type(Args&&... args) : base(std::forward<Args>(args)...) {}

namespace nibbles { namespace utility {

namespace mpl = boost::mpl;
namespace fusion = boost::fusion;

// Helper wrapper to distinguish different sorts of template args passed to
// DataClass
template<typename Base>
struct InheritFrom {
  typedef Base type;
};

namespace detail {

// Helper metafunction to construct the field map out of the list of
// type/names given; basically this means taking them in pairs
// See below for definition
template<typename... Fields>
struct FieldMapHelper;

// Ditto for the base class
template<typename... Fields>
struct BaseClassHelper;

// Ditto for the mpl sequence of base classes which make up the base class
template<typename... Fields>
struct BaseClassSequenceHelper;

template<typename Class, typename FieldMap, typename BaseClassSequence>
struct FieldClassMapHelper;

struct EmptyBase;

// Return a type inheriting from both args, and with a constructor which can
// construct It Right
template<typename T1, typename T2>
struct Inherit;

}

template<typename Derived, typename... Fields>
class DataClass : public detail::BaseClassHelper<Fields...>::type {
  friend class boost::serialization::access;
  private:
    typedef typename detail::FieldMapHelper<Fields...>::type FieldMapSequence;
    static const size_t numFields_ =
      fusion::result_of::size<FieldMapSequence>::type::value;
    typedef typename detail::BaseClassSequenceHelper<Fields...>::type
      BaseClassSequence;
    static const size_t numBases_ = mpl::size<BaseClassSequence>::type::value;
    typedef typename detail::BaseClassHelper<Fields...>::type BaseOfDataClass;
  public:
    typedef typename fusion::result_of::as_map<
        FieldMapSequence
      >::type FieldMap;
    typedef typename detail::FieldClassMapHelper<
        Derived,
        FieldMap,
        BaseClassSequence
      >::type FieldClassMap;
    typedef DataClass base;
  private:
    template<typename Field>
    struct TypeOfField {
      static_assert(
          IsField<Field>::type::value,
          "tag is not a field"
        );
      static_assert(
          mpl::has_key<FieldClassMap, Field>::type::value,
          "no such field in DataClass"
        );
      typedef typename mpl::at<FieldClassMap, Field>::type FieldClass;
      typedef typename fusion::result_of::at_key<
          typename FieldClass::FieldMap,
          Field
        >::type type;
    };

    FieldMap fields_;
  public:
    DataClass()
    {
    }

    template<typename Arg>
    DataClass(
        Arg&& copy,
        typename boost::enable_if<
          typename std::is_base_of<
            DataClass,
            typename std::remove_const<
              typename std::remove_reference<Arg>::type
            >::type
          >::type,
          int
        >::type = 0
      ) :
      BaseOfDataClass(static_cast<BaseOfDataClass const&>(copy)),
      fields_(static_cast<DataClass const&>(copy).fields_)
    {
    }

    template<typename... Args>
    DataClass(Args&&... args) :
      detail::BaseClassHelper<Fields...>::type(
          singletonOrSequence(slice<0, numBases_>(std::forward<Args>(args)...))
        ),
      fields_(
          slice<numBases_, numBases_+numFields_>(std::forward<Args>(args)...)
        )
    {
      static_assert(
          numBases_+numFields_ == sizeof...(Args),
          "wrong number of arguments to constructor"
        );
    }

    template<typename Field>
    typename boost::call_traits<
        typename TypeOfField<Field>::type
      >::reference get() {
      typedef typename mpl::at<FieldClassMap, Field>::type::base FieldClass;
      return dynamic_cast<FieldClass&>(*this).local_get<Field>();
    }

    template<typename Field>
    typename boost::call_traits<
        typename TypeOfField<Field>::type
      >::const_reference get() const {
      typedef typename mpl::at<FieldClassMap, Field>::type::base FieldClass;
      return dynamic_cast<FieldClass const&>(*this).local_get<Field>();
    }

    template<typename Field>
    typename boost::call_traits<
        typename fusion::result_of::at_key<FieldMap, Field>::type
      >::reference local_get() {
      return fusion::at_key<Field>(fields_);
    }

    template<typename Field>
    typename boost::call_traits<
        typename fusion::result_of::at_key<FieldMap, Field>::type
      >::const_reference local_get() const {
      return fusion::at_key<Field>(fields_);
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

    template<class Archive>
    void serialize(Archive& ar, unsigned int const /*version*/)
    {
      ar & boost::serialization::make_nvp(
          "base", boost::serialization::base_object<BaseOfDataClass>(*this)
        );
      fusion::for_each(fields_, Archiver<Archive>(ar));
    }
};

namespace detail {

template<>
struct FieldMapHelper<> {
  typedef fusion::list<> type;
};

template<typename Leftover>
struct FieldMapHelper<Leftover> {
  static_assert(
      sizeof(Leftover) == -1,
      "odd number of field template args given"
    );
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
  typedef typename fusion::result_of::push_front<
      MapSoFar,
      fusion::pair<FieldName, FieldType>
    >::type type;
};

template<typename BaseType, typename Next, typename... More>
struct FieldMapHelper<InheritFrom<BaseType>, Next, More...>
{
  typedef typename FieldMapHelper<Next, More...>::type type;
};

template<typename... Fields>
struct BaseClassHelper {
  typedef typename mpl::inherit_linearly<
      typename BaseClassSequenceHelper<Fields...>::type,
      Inherit<mpl::_2, mpl::_1>,
      EmptyBase
    >::type type;
};

template<>
struct BaseClassSequenceHelper<> {
  typedef mpl::vector<>::type type;
};

template<typename BaseType, typename... More>
struct BaseClassSequenceHelper<InheritFrom<BaseType>, More...> {
  typedef typename mpl::push_front<
      typename BaseClassSequenceHelper<More...>::type,
      BaseType
    >::type type;
};

template<typename Field, typename... More>
struct BaseClassSequenceHelper<Field, More...> {
  typedef typename BaseClassSequenceHelper<More...>::type type;
};

template<typename Class, typename FieldMap, typename BaseClassSequence>
struct FieldClassMapHelper {
  template<typename DataClass>
  struct GetFieldClassMap {
    typedef typename DataClass::FieldClassMap type;
  };

  template<typename MapToAddTo, typename MapToGetFrom>
  struct AddWhereMissing {
    typedef typename mpl::fold<
        MapToGetFrom,
        MapToAddTo,
        mpl::if_<
          mpl::has_key<mpl::_1, mpl::first<mpl::_2>>,
          mpl::_1,
          mpl::insert<mpl::_1, mpl::_2>
        >
      >::type type;
  };

  typedef typename mpl::transform<
      BaseClassSequence,
      GetFieldClassMap<mpl::_1>
    >::type BaseFieldClassMaps;
  typedef typename mpl::fold<
      FieldMap,
      mpl::map<>::type,
      mpl::insert<
        mpl::_1,
        mpl::pair<fusion::result_of::first<mpl::_2>, Class>
      >
    >::type MyFieldClassMap;
  typedef typename mpl::fold<
      BaseFieldClassMaps,
      MyFieldClassMap,
      AddWhereMissing<mpl::_1, mpl::_2>
    >::type type;
};

struct EmptyBase {
  EmptyBase() {}

  EmptyBase(const EmptyBase&) {}

  template<typename Sequence>
  EmptyBase(
      const Sequence&,
      typename boost::disable_if<
        std::is_base_of<EmptyBase, Sequence>,
        int
      >::type = 0
    ) {
    static_assert(
        fusion::traits::is_sequence<Sequence>::type::value,
        "argument not a fusion sequence"
      );
    static_assert(
        fusion::result_of::size<Sequence>::type::value == 0,
        "argument is not empty"
      );
  }

  template<class Archive>
  void serialize(Archive&, unsigned int const /*version*/)
  {
  }
};

template<typename T1, typename T2>
struct Inheriter : T1, T2 {
  template<typename Sequence>
  Inheriter(Sequence&& seq) :
    T1(fusion::front(seq)),
    T2(singletonOrSequence(slice<1>(seq)))
  {}
};

template<typename T1, typename T2>
struct Inherit {
  typedef Inheriter<T1, T2> type;
};

template<typename T1>
struct Inherit<T1, EmptyBase> {
  typedef T1 type;
};

}

}}

#endif // NIBBLES__UTILITY__DATACLASS_HPP

