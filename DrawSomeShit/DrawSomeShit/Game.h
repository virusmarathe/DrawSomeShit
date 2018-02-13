#pragma once

#include "SDL.h"
#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <vector>
#include <map>
#include "PencilObject.h"
#include "Vector2.h";
#include <winsock.h>

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
	bool mIsMouseDown;
	GameObject * lastCreatedObject;

	// need switch this to an object pool so you don't create every object at runtime
	std::vector<GameObject*> mActiveGameObjectList;
	bool keysPressed[SDL_NUM_SCANCODES];
};

