#ifndef NIBBLES__UTILITY__IDBASE_HPP
#define NIBBLES__UTILITY__IDBASE_HPP

#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>

namespace nibbles { namespace utility {

/** \brief Helper class to allow id-style classes to be easily constructed.
 *
 * Classes can inherit from this class and have semantics appropriate to an id.
 * The template parameters should be some integral type and the type of the
 * derived class itself.  See PlayerId for an example.
 *
 * IdBase adds as much appropriate functionality as it can to the derived
 * class through the use of friend function definitions and static methods.
 *
 * Thanks to the way boost::hash works we can also have hashes work
 * automatically for all derived classes.
 *
 * boost::serialization should also work by magic.
 *
 * It is guaranteed that if an instance is default-constructed and then
 * incremented, all values obtained will be distinct until an invalid value is
 * reached (as determined by the \c valid method); after that duplicates may be
 * observed.
 */
template<typename TInteger, typename TDerived>
class IdBase {
  static_assert(
      std::numeric_limits<TInteger>::is_integer,
      "internal id type must be an integer type"
    );
  friend class boost::serialization::access;
  public:
    /** \brief Type which this is a wrapper for. */
    typedef TInteger internal_type;

    /** \brief Implicit cast to internal type.
     *
     * This allows this to be treated as an integer for such things as
     * comparisons and array indexing. */
    operator TInteger() const { return val; }
    /** \brief Determine if this id is valid. */
    bool valid() const { return val != TInteger(-1); }
    /** \brief Convert to string.
     *
     * \see IdBase::fromString */
    std::string toString() const {
      return boost::lexical_cast<std::string>(val);
    }

    /** \brief Construct an invalid id. */
    static TDerived invalid() {
      TDerived temp; temp.val=TInteger(-1); return temp;
    }
    /** \brief Construct from string.
     *
     * It is guaranteed that converting an id to a string and back again will
     * yield the original value.
     *
     * \see IdBase::toString. */
    static TDerived fromString(const std::string& s) {
      return fromInteger(boost::lexical_cast<TInteger>(s));
    }
    /** \brief Construct from internal integer representation.
     *
     * The need to use this function is a bad sign; it shows excessive
     * dependence on the internal representation. */
    static TDerived fromInteger(const TInteger i) {
      TDerived b;
      b.val = i;
      return b;
    }

    friend TDerived& operator++(TDerived& b) { ++b.val; return b; }
    friend TDerived operator++(TDerived& b, int) {
      TDerived temp(b); ++b; return temp;
    }
    friend bool operator==(TDerived l, TDerived r) { return l.val == r.val; }
    friend bool operator!=(TDerived l, TDerived r) { return l.val != r.val; }
  protected:
    /** \brief Default constructor always produces the same, valid id */
    IdBase() : val(0) {}
  private:
    TInteger val;

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
      ar & BOOST_SERIALIZATION_NVP(val);
    }
};

/* The name hash_value for this function is required for it to work with
 * boost::hash */
template<typename TInteger, typename TDerived>
inline size_t hash_value(const IdBase<TInteger, TDerived>& id)
{
  boost::hash<TInteger> intHasher;
  return intHasher(id);
};

}}

#endif // NIBBLES__UTILITY__IDBASE_HPP

