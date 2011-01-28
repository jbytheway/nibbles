#include <nibbles/leveldefinition.hpp>

#include <random>

#include <boost/foreach.hpp>

namespace nibbles {

LevelDefinition LevelDefinition::realise() const
{
  LevelDefinition result(*this);
  std::random_device device;
  std::mt19937 random(device);
  BOOST_FOREACH(auto const& block_prob, result.get<randomBlocks>()) {
    std::bernoulli_distribution dist(block_prob.second);
    BOOST_FOREACH(const Point& p, block_prob.first) {
      if (dist(random)) {
        result.get<blocks>().push_back({p.get<x>(), p.get<y>(), 1, 1});
      }
    }
  }
  result.get<randomBlocks>().clear();
  return result;
}

}

