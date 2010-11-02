#ifndef NIBBLES__GAMEEVENTHANDLER_HPP
#define NIBBLES__GAMEEVENTHANDLER_HPP

#include <nibbles/leveldefinition.hpp>

namespace nibbles {

class GameEventHandler {
  public:
    GameEventHandler(GameEventHandler const&) = delete;
    GameEventHandler& operator=(GameEventHandler const&) = delete;

    virtual void startLevel(LevelDefinition const&) = 0;
  protected:
    GameEventHandler();
    virtual ~GameEventHandler() = 0;
};

}

#endif // NIBBLES__GAMEEVENTHANDLER_HPP

