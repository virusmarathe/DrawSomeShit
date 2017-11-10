#pragma once
#include "oxygine-framework.h"
#include <list>
using namespace oxygine;

DECLARE_SMART(Game, spGame);

class Game: public Actor
{
public:
    Game();
    ~Game();

    void init();

protected:

    void doUpdate(const UpdateState& us);
};