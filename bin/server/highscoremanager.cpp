#include "highscoremanager.hpp"

#include <boost/scope_exit.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <nibbles/fatal.hpp>

namespace nibbles { namespace server {

HighScoreManager::HighScoreManager(boost::filesystem::path const& path) :
  file_(path)
{
  if (boost::filesystem::exists(file_)) {
    boost::filesystem::ifstream ifs(file_);
    if (!ifs.is_open()) {
      throw std::runtime_error(
        "failed to open high scores "+file_.file_string()
      );
    }
    boost::archive::xml_iarchive ia(ifs);
    ia >> BOOST_SERIALIZATION_NVP(scores_);
  }
}

HighScoreReport HighScoreManager::insert(
  GameSettings const& settings,
  HighScore score
)
{
  HighScoreSet& scoreSet = scores_[settings];
  HighScoreSet::iterator insertedIt = scoreSet.insert(score).first;

  HighScoreReport result;
  size_t rank = 1;
  bool gotNew = false;
  for (auto s = scoreSet.rbegin(); s != scoreSet.rend(); ++s, ++rank) {
    if (rank <= 10 || &*s == &*insertedIt) {
      result.get<scores>().push_back({rank, *s});
      if (&*s == &*insertedIt) {
        gotNew = true;
      }
    }
    if (rank > 10 && gotNew) break;
  }
  return result;
}

void HighScoreManager::save() const
{
  boost::filesystem::path tempFile = file_.string()+".tmp";
  BOOST_SCOPE_EXIT((&tempFile)) {
    boost::filesystem::remove(tempFile);
  } BOOST_SCOPE_EXIT_END
  {
    boost::filesystem::ofstream ofs(tempFile);
    if (!ofs.is_open()) {
      throw std::runtime_error(
        "failed to open temp high scores for saving at "+tempFile.file_string()
      );
    }
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(scores_);
  }
  // Can't use boost::filesystem::rename beacuse it incorrectly errors when
  // destination file exists, see https://svn.boost.org/trac/boost/ticket/2866
  // (FIXME: fix with v3)
  //boost::filesystem::rename(tempFile, file_);
  if (0 != ::rename(
      tempFile.file_string().c_str(), file_.file_string().c_str()
    )) {
    NIBBLES_FATAL("error renaming, errno=" << errno);
  }
}

}}

