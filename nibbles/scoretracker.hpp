#ifndef NIBBLES__SCORETRACKER_HPP
#define NIBBLES__SCORETRACKER_HPP

#include <map>

#include <nibbles/scorehandler.hpp>

namespace nibbles {

class ScoreTracker : public ScoreHandler {
  public:
    void add(PlayerId const);
    Score operator[](PlayerId const) const;
    virtual void operator()(PlayerId const, Score const);
  private:
    std::map<PlayerId, Score> scores_;
};

}

#endif // NIBBLES__SCORETRACKER_HPP

