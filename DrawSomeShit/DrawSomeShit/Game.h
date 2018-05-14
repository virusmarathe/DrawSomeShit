#pragma once
#define GLEW_STATIC

#include "SDL.h"
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <IL/il.h>
#include <IL/ilu.h>
#include "StateMachine.h"
#include "PencilObject.h"
#include "Vector2.h"
#include "NetworkManager.h"
#include "TextureObject.h"
#include "FontSheet.h";
#include "TextObject.h"

class Game
{
public:
	Game();
	~Game();

	static Game * Instance() { return sInstance; }

	// initialize systems
	void init(const char * title, int xPos, int yPos, int width, int height, bool fullscreen);

	// handle user input
	void handleEvents();

	void update();

	void render();

	void clean();

	bool running() { return mIsRunning; }

	int GetPlayerID() { return mPlayerID; }

private:
	void setupOpenGL(int width, int height);
	void loadMedia();
	void setupConnection();
	void processCommandBuffer(charbuf &buf, int bufSize);

private:
	static Game * sInstance;

	bool mIsRunning;
	SDL_Window * mWindow;
	SDL_GLContext mContext;
	Uint32 mLastFrameTime;
	bool mIsMouseDown;
	int mScreenWidth;
	int mScreenHeight;

	// need switch this to an object pool so you don't create every object at runtime
	std::vector<GameObject*> mActiveGameObjectList;
	std::vector<GameObject*> mTextObjects;
	bool keysPressed[SDL_NUM_SCANCODES];

	FontSheet * mTestFontSheet;
	GameObject * mNextTextObject;

	std::vector<GameObject *> mNetworkedGameObjectList;
	std::map<int, GameObject*> mNetworkedGameObjectMap;
	int mPlayerID;

	bool mEnterWasPressed = true;

	std::vector<std::string> mWordlist;
	GameObject * mCurrentWord;

	StateMachine<Game> * mStateMachine;
};

