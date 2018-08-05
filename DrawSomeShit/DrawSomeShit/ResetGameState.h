#pragma once
#include "State.h"

class Game;
class ResetGameState :	public State<Game>
{
public:
	ResetGameState(bool isServer);
	~ResetGameState();

	virtual void Enter(Game*);
	virtual void Update(Game*, float deltaTime);
	virtual void Exit(Game*);

private:
	float mTimer;
};

