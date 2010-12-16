#include <optimal/optionsparser.hpp>

#include <boost/archive/xml_oarchive.hpp>

#include <nibbles/levelpack.hpp>

#include "levels.hpp"

int main(int argc, char const* const* const argv)
{
  bool classic;

  optimal::OptionsParser parser;
  parser.addOption("classic", 'c', &classic);
  parser.parse(argc, argv);

  nibbles::LevelPack levels;

  if (classic) {
    levels = nibbles::levels::classicLevels();
  } else {
    levels = nibbles::levels::ultraLevels();
  }

  boost::archive::xml_oarchive oa(std::cout);
  oa << boost::serialization::make_nvp("levelPack_", levels);

  return 0;
}

