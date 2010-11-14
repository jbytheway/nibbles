#ifndef NIBBLES__GAMEEVENTHANDLER_HPP
#define NIBBLES__GAMEEVENTHANDLER_HPP

#include <nibbles/leveldefinition.hpp>
#include <nibbles/number.hpp>
#include <nibbles/moves.hpp>

namespace nibbles {

class GameEventHandler {
  public:
    GameEventHandler(GameEventHandler const&) = delete;
    GameEventHandler& operator=(GameEventHandler const&) = delete;

    virtual void startLevel(LevelDefinition const&) = 0;
    virtual void newNumber(Number const&) = 0;
    virtual void tick(Moves const&) = 0;
  protected:
    GameEventHandler();
    virtual ~GameEventHandler() = 0;
};

}

#endif // NIBBLES__GAMEEVENTHANDLER_HPP

