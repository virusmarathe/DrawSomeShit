#include "DrawingGameState.h"
#include "Game.h"

const float ROUND_TIME = 60.0f;

DrawingGameState::DrawingGameState(bool isServer) : State(isServer)
{
}


DrawingGameState::~DrawingGameState()
{
}


void DrawingGameState::Enter(Game * gameRef)
{
	mfTimer = 0.0f;
	std::string nextWord = gameRef->GetNextWord();
	mCachedCurrentWord = nextWord;
	mDrawerPlayerID = gameRef->GetCurrentDrawerPlayerID();
	if (gameRef->GetCurrentDrawerPlayerID() == gameRef->GetPlayerID())
	{
		gameRef->SetCurrentWord("Draw: " + nextWord);
	}
	else
	{
		nextWord.assign(nextWord.length(), '-');
		gameRef->SetCurrentWord(nextWord);
	}
	wordGuessed = false;
}

void DrawingGameState::Update(Game * gameRef, float deltaTime)
{
	if (mIsServer)
	{
		mfTimer += deltaTime;
		if (mfTimer > ROUND_TIME || wordGuessed)
		{
			gameRef->ChangeGameStateServer(GameState::SelectWord);
		}
	}
}

void DrawingGameState::Exit(Game * gameRef)
{
}

void DrawingGameState::RecieveGuess(std::string guess, int playerID)
{
	if (playerID == mDrawerPlayerID) return;

	int index = guess.find_first_of(':');
	guess = guess.substr(index+2, guess.length());
	if (mIsServer)
	{
		if (guess == mCachedCurrentWord)
		{
			wordGuessed = true;
		}
	}
}
