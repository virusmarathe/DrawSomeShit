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
	gameRef->SetCurrentWord("Choose a word...");
}

void SelectWordGameState::Update(Game * gameRef, float deltaTime)
{
}

void SelectWordGameState::Exit(Game * gameRef)
{
}
