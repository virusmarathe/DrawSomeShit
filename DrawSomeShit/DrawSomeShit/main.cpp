#include "Game.h"

Game * mGame = NULL;

int main(int argc, char * argv[])
{
	mGame = new Game();
	
	mGame->init("Draw Some Shit!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

	// game loop
	while (mGame->running())
	{
		// handle input
		mGame->handleEvents();

		// update
		mGame->update();

		// render
		mGame->render();
	}

	mGame->clean();
	
	delete mGame;

	return 0;
}