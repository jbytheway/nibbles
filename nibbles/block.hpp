#ifndef NIBBLES__BLOCK_HPP
#define NIBBLES__BLOCK_HPP

#include <boost/iterator/iterator_facade.hpp>

#include <nibbles/point.hpp>

namespace nibbles {

struct Block :
  utility::DataClass<
    Block,
    Point, fields::min,
    Point, fields::max
  > {
  NIBBLES_UTILITY_DATACLASS_CONSTRUCTOR(Block)

  template<typename Int0, typename Int1, typename Int2, typename Int3>
  Block(Int0 x, Int1 y, Int2 w, Int3 h) :
    base(Point(x, y), Point(x+w, y+h)) {}

  Block& operator-=(const Point& p) {
    get<min>() -= p;
    get<max>() -= p;
    return *this;
  }

  Point::value_type height() const {
    return get<max>().get<y>() - get<min>().get<y>();
  }

  class iterator :
    public boost::iterator_facade<
      iterator,
      const Point,
      boost::random_access_traversal_tag
    > {
    friend class boost::iterator_core_access;
    friend class Block;
    private:
      explicit iterator(const Block& b) :
        block(&b), point(b.get<min>()) {}
      iterator(const Block& b, const Point& p) :
        block(&b), point(p) {}

      const Block* block;
      Point point;

      const Point& dereference() const {
        return point;
      }

      bool equal(const iterator& right) const {
        return (point == right.point);
      }

      void increment() {
        ++point.get<y>();
        if (point.get<y>() == block->get<max>().get<y>()) {
          point.get<y>() = block->get<min>().get<y>();
          ++point.get<x>();
        }
      }

      void decrement() {
        --point.get<y>();
        if (point.get<y>() < block->get<min>().get<y>()) {
          point.get<y>() = block->get<max>().get<y>()-1;
          --point.get<x>();
        }
      }

      void advance(ptrdiff_t n) {
        point.get<y>() += n;
        while (point.get<y>() < block->get<min>().get<y>()) {
          point.get<y>() += block->height();
          --point.get<x>();
        }
        while (point.get<y>() >= block->get<max>().get<y>()) {
          point.get<y>() -= block->height();
          ++point.get<x>();
        }
      }
  };

  typedef iterator const_iterator;

  iterator begin() const {
    return iterator(*this);
  }

  iterator end() const {
    return iterator(*this, Point(get<max>().get<x>(), get<min>().get<y>()));
  }
};

}

#endif // NIBBLES__BLOCK_HPP

