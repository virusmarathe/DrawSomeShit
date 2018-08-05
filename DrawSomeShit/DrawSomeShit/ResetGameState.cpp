#include "ResetGameState.h"
#include "Game.h"

ResetGameState::ResetGameState(bool isServer) : State(isServer)
{
	mTimer = 0.0f;
}


ResetGameState::~ResetGameState()
{
}

void ResetGameState::Enter(Game * gameRef)
{
	mTimer = 0.0f;
	gameRef->SetCurrentWord("Welcome to Draw Some Shit!");
}

void ResetGameState::Update(Game * gameRef, float deltaTime)
{
	if (mIsServer)
	{
		mTimer += deltaTime;
		if (mTimer > 10.0f)
		{
			gameRef->ChangeGameStateServer(GameState::SelectWord);
		}
	}
}

void ResetGameState::Exit(Game * gameRef)
{
}
