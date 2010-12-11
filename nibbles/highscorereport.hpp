#ifndef NIBBLES__HIGHSCOREREPORT_HPP
#define NIBBLES__HIGHSCOREREPORT_HPP

#include <nibbles/utility/dataclass.hpp>
#include <nibbles/highscore.hpp>

namespace nibbles {

struct HighScoreReport :
  utility::DataClass<
    HighScoreReport,
    std::vector<std::pair<size_t, HighScore>>, scores
  > {
};

}

#endif // NIBBLES__HIGHSCOREREPORT_HPP

