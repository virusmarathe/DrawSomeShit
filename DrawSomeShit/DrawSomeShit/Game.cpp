#include "Game.h"

Game::Game() : mIsRunning(false)
{
}


Game::~Game()
{
}

void Game::init(const char * title, int xPos, int yPos, int width, int height, bool fullscreen)
{
	// initialize SDL subsystems, window, and renderer
	int flags = 0;
	if (fullscreen)	flags |= SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "SDL subsystem initialized..." << std::endl;

		mWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
		if (mWindow == NULL)
		{
			std::cout << "Failed to create window!" << std::endl;
			return;
		}

		mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
		if (mRenderer == NULL)
		{
			std::cout << "Failed to create renderer!" << std::endl;
			return;
		}
		
		mIsRunning = true;
	}
	else
	{
		std::cout << "Failed to initialize SDL subsystems!" << std::endl;
		return;
	}
}

void Game::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT:
			mIsRunning = false;
			break;

		default:
			break;
	}
}

void Game::update()
{
}

void Game::render()
{
	// clear buffer
	SDL_RenderClear(mRenderer);

	// render objects
	SDL_RenderPresent(mRenderer);
}

void Game::clean()
{
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();

	std::cout << "Game cleaned" << std::endl;
}
