#ifndef NIBBLES__SCOREHANDLER_HPP
#define NIBBLES__SCOREHANDLER_HPP

#include <nibbles/playerid.hpp>
#include <nibbles/score.hpp>

namespace nibbles {

class ScoreHandler {
  public:
    virtual void addScore(PlayerId const, Score const) = 0;
    // Return true iff lives exhausted
    virtual bool deductLife(PlayerId const) = 0;
  protected:
    virtual ~ScoreHandler() = 0;
};

}

#endif // NIBBLES__SCOREHANDLER_HPP

