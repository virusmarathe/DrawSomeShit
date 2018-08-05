#include "SelectWordGameState.h"
#include "Game.h"

SelectWordGameState::SelectWordGameState(bool isServer) : State(isServer)
{
}

SelectWordGameState::~SelectWordGameState()
{
}

void SelectWordGameState::Enter(Game * gameRef)
{
	if (mIsServer)
	{
		gameRef->UpdateNextDrawer();
		gameRef->UpdateNextWord();
	}
	gameRef->SetCurrentWord("Selecting next word...");
	mfTimer = 0.0f;
}

void SelectWordGameState::Update(Game * gameRef, float deltaTime)
{
	if (mIsServer)
	{
		mfTimer += deltaTime;
		if (mfTimer > 5.0f)
		{
			gameRef->ChangeGameStateServer(GameState::Drawing);
		}
	}
}

void SelectWordGameState::Exit(Game * gameRef)
{
}
