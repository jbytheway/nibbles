#ifndef NIBBLES__SCORETRACKER_HPP
#define NIBBLES__SCORETRACKER_HPP

#include <map>

#include <nibbles/lifecount.hpp>
#include <nibbles/scorehandler.hpp>

namespace nibbles {

class ScoreTracker : public ScoreHandler {
  public:
    void add(PlayerId const, LifeCount const);
    void clear();
    Score getScore(PlayerId const) const;
    LifeCount getLives(PlayerId const) const;
    virtual void addScore(PlayerId const, Score const);
    // Return true iff lives exhausted
    virtual bool deductLife(PlayerId const);
  private:
    std::map<PlayerId, Score> scores_;
    std::map<PlayerId, LifeCount> lives_;
};

}

#endif // NIBBLES__SCORETRACKER_HPP

