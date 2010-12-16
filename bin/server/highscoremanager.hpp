#ifndef NIBBLES_SERVER__HIGHSCOREMANAGER_HPP
#define NIBBLES_SERVER__HIGHSCOREMANAGER_HPP

#include <set>

#include <boost/filesystem/path.hpp>

#include <nibbles/highscorereport.hpp>
#include <nibbles/gamesettings.hpp>

namespace nibbles { namespace server {

class HighScoreManager {
  public:
    HighScoreManager(boost::filesystem::path const&);

    HighScoreReport insert(GameSettings const&, HighScore);

    void save() const;
  private:
    boost::filesystem::path file_;

    typedef std::pair<uint32_t, GameSettings> GameType;
    typedef std::set<HighScore> HighScoreSet;
    std::map<GameType, HighScoreSet> scores_;
};

}}

#endif // NIBBLES_SERVER__HIGHSCOREMANAGER_HPP

