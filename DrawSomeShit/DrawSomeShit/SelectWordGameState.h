#pragma once
#include "State.h"

class Game;
class SelectWordGameState :	public State<Game>
{
public:
	SelectWordGameState(bool isServer);
	~SelectWordGameState();

	virtual void Enter(Game*);
	virtual void Update(Game*, float deltaTime);
	virtual void Exit(Game*);

private:
	float mfTimer;
};

