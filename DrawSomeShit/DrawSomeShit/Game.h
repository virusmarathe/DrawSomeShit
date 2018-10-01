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
#include "EventManager.h"
#include "PencilObject.h"
#include "Vector2.h"
#include "NetworkManager.h"
#include "TextureObject.h"
#include "FontSheet.h";
#include "TextObject.h"
#include "SelectWordGameState.h"
#include "DrawingGameState.h"
#include "ResetGameState.h"

enum GameState
{
	Reset,
	SelectWord,
	Drawing
};

class Game : public IEventListener
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

	void ChangeGameStateServer(GameState state);
	void ChangeGameStateLocal(GameState state);

	void SetCurrentWord(std::string str);

	bool isServer() { return mIsServer; }

	void OnNewPlayerConnected(int id);

	void UpdateNextDrawer();

	int GetCurrentDrawerPlayerID() { return mCurrentDrawerPlayerID; }

	void UpdateNextWord();
	std::string GetNextWord() {	return mNextWord; }

	void ClearDrawings();

	void SetTimerText(int val);
	
	virtual void HandleEvent(EventParam * param);

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
	TextObject * mNextTextObject;

	std::vector<GameObject *> mNetworkedGameObjectList;
	std::map<int, GameObject*> mNetworkedGameObjectMap;
	int mPlayerID;

	bool mEnterWasPressed = true;

	std::vector<std::string> mWordlist;
	TextObject * mCurrentWord;
	TextObject * mTimerText;

	StateMachine<Game> * mStateMachine;
	std::map<GameState, State<Game>*> mGameStates;
	GameState mCurrentGameStateID;

	bool mIsServer;

	std::vector<int> mPlayerOrder;
	int mCurrentPlayerIndex;
	int mCurrentDrawerPlayerID;
	std::string mNextWord;

	EventManager * mEventManager;
};

