#include <optimal/optionsparser.hpp>

#include <boost/archive/xml_oarchive.hpp>

#include <nibbles/levelpack.hpp>

#include "levels.hpp"

using namespace std;
using namespace nibbles;
using namespace nibbles::levels;

int main(int argc, char const* const* const argv)
{
  bool classic;

  optimal::OptionsParser parser;
  parser.addOption("classic", 'c', &classic);
  parser.parse(argc, argv);

  LevelPack levels;

  if (classic) {
    levels = classicLevels();
  } else {
    levels = ultraLevels();
  }

  boost::archive::xml_oarchive oa(cout);
  oa << boost::serialization::make_nvp("levelPack_", levels);

  return 0;
}

