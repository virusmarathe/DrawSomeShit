#pragma once

#include "SDL.h"
#include <iostream>

class Game
{
public:
	Game();
	~Game();

	// initialize systems
	void init(const char * title, int xPos, int yPos, int width, int height, bool fullscreen);

	// handle user input
	void handleEvents();

	void update();

	void render();

	void clean();

	bool running() { return mIsRunning; }

private:
	bool mIsRunning;
	SDL_Window * mWindow;
	SDL_Renderer * mRenderer;
};

