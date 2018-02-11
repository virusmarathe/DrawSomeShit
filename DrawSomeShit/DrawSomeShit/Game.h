#pragma once

#include "SDL.h"
#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <vector>
#include "GameObject.h"

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
	void setupOpenGL(int width, int height);

private:
	bool mIsRunning;
	SDL_Window * mWindow;
	SDL_GLContext mContext;
	Uint32 mLastFrameTime;

	std::vector<GameObject*> mActiveGameObjectList;
};

