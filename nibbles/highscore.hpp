#ifndef NIBBLES__HIGHSCORE_HPP
#define NIBBLES__HIGHSCORE_HPP

#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/fields.hpp>
#include <nibbles/scoretracker.hpp>

namespace nibbles {

struct HighScore :
  utility::DataClass<
    HighScore,
    boost::posix_time::ptime, fields::time, // UTC
    Score, totalScore,
    std::map<std::string, Score>, playerScores
  > {
  HighScore() = default;
  template<typename Players>
  HighScore(Players const&, ScoreTracker const&);
};

template<typename Players>
HighScore::HighScore(Players const& players, ScoreTracker const& scores) :
  base(
    boost::posix_time::second_clock::universal_time(),
    0,
    std::map<std::string, Score>()
  )
{
  auto& playerScores = get<fields::playerScores>();
  BOOST_FOREACH(auto const& player, players) {
    PlayerId const id = player.id();
    std::string const name = player.get<fields::name>();
    Score const score = scores.getScore(id);

    get<totalScore>() += score;
    playerScores.insert(std::make_pair(name, score));
  }
}

bool operator<(HighScore const&, HighScore const&);

}

#endif // NIBBLES__HIGHSCORE_HPP

