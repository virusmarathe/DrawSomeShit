#pragma once

#include "SDL.h"
#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include "PencilObject.h"
#include "Vector2.h"
#include "NetworkManager.h"

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

	void SendNetworkMessage(charbuf & dataBuf);
	void ForwardMessageToClients(charbuf & dataBuf, int clientIndex);

private:
	void setupOpenGL(int width, int height);
	void setupConnection();
	void handleNetworkData();

private:
	static Game * sInstance;

	bool mIsRunning;
	SDL_Window * mWindow;
	SDL_GLContext mContext;
	Uint32 mLastFrameTime;
	bool mIsMouseDown;

	// need switch this to an object pool so you don't create every object at runtime
	std::vector<GameObject*> mActiveGameObjectList;
	bool keysPressed[SDL_NUM_SCANCODES];

	// networking variables
	bool mConnected;
	ConnectionType mConnectionType;
	ClientSocketTCP * mTCPClient;
	ConnectionInfo * mRemoteIP;
	NetworkMessage mMsg;
	HostSocketTCP * mTCPListener;

	std::vector<GameObject *> mNetworkedGameObjectList;
	std::map<int, GameObject*> mNetworkedGameObjectMap;
	int mPlayerID;
	int mObjectIDCounter;
};

