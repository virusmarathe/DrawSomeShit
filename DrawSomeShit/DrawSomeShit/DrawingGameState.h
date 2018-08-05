#pragma once
#include "State.h"
#include <string>
class Game;

class DrawingGameState :
	public State<Game>
{
public:
	DrawingGameState(bool isServer);
	~DrawingGameState();

	virtual void Enter(Game*);
	virtual void Update(Game*, float deltaTime);
	virtual void Exit(Game*);

	void RecieveGuess(std::string guess, int playerID);

private:
	float mfTimer;
	std::string mCachedCurrentWord;
	bool wordGuessed;
	int mDrawerPlayerID;
};

