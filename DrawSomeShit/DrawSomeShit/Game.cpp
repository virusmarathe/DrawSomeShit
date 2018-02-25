#include "Game.h"

#define PORT 6969

Game::Game() : mIsRunning(false), mLastFrameTime(0), mIsMouseDown(false)
{
	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		keysPressed[i] = false;
	}

	mTCPListener = NULL;
	mTCPClient = NULL;
	mConnected = false;
	mRemoteIP = NULL;
	mConnectionType = ConnectionType::NONE;
}


Game::~Game()
{
}

void Game::init(const char * title, int xPos, int yPos, int width, int height, bool fullscreen)
{
	// initialize SDL subsystems, window, and renderer
	int flags = 0;
	if (fullscreen)	flags |= SDL_WINDOW_FULLSCREEN;
	int status = SDL_Init(SDL_INIT_EVERYTHING);
	if (status != 0)
	{
		std::cout << "Failed to initialize SDL subsystems!" << std::endl;
		return;
	}
	std::cout << "SDL subsystem initialized..." << std::endl;
	mIsRunning = true;

	NetworkManager::Init();
	setupConnection();

	// setup window attributes for OpenGL window
	// we want at least 5 bits of RGB, and a 16 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	flags |= SDL_WINDOW_OPENGL;

	mWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create window!" << std::endl;
		return;
	}		

	// initialize opengl
	mContext = SDL_GL_CreateContext(mWindow);
	if (mContext == NULL)
	{
		std::cout << "Failed to create openGL renderer!" << std::endl;
		return;
	}
	setupOpenGL(width, height);
	mLastFrameTime = SDL_GetTicks();
}

void Game::setupOpenGL(int width, int height)
{
	// clear out projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);

	// clear out model matrix matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 1);
}

void Game::setupConnection()
{
	int num = 0;
	char ipString[32];

	while (num == 0)
	{
		// temporary menu for server/client info getting
		std::cout << "Some Shit Menu:\n1) Host Game\n2) Join Game\n3) Exit" << std::endl;
		std::cin >> num;
		switch (num)
		{
		case 1:
			mConnectionType = ConnectionType::HOST;
			break;
		case 2:
			std::cout << "Enter the host ip: ";
			std::cin >> ipString;
			mConnectionType = ConnectionType::CLIENT;
			break;
		case 3:
			mIsRunning = false;
			break;
		default:
			std::cout << "Please enter a valid selection." << std::endl;
			num = 0;
			break;
		}
	}

	if (mConnectionType == ConnectionType::HOST)
	{
		mTCPListener = new HostSocketTCP(PORT);
		if (!mTCPListener->Valid())
		{
			exit(EXIT_FAILURE);
		}
		mTCPClient = new ClientSocketTCP();
	}
	else if (mConnectionType == ConnectionType::CLIENT)
	{
		mTCPClient = new ClientSocketTCP();

		mRemoteIP = new ConnectionInfo(ipString, PORT);
	}

}

void Game::handleNetworkData()
{
	if (!mConnected)
	{
		if (mConnectionType == ConnectionType::HOST)
		{
			if (mTCPListener->Accept(*mTCPClient))
			{
				mConnected = true;
			}
		}
		else if (mConnectionType == ConnectionType::CLIENT)
		{
			if (mTCPClient->Connect(*mRemoteIP))
			{
				if (mTCPClient->Valid())
				{
					mConnected = true;
				}
			}
		}
	}
	else
	{
		if (mTCPClient->Ready())
		{
			if (mTCPClient->Recieve(mMsg))
			{
				char buf[256];
				mMsg.UnLoadBytes(buf);
				std::cout << buf;
			}
			else
			{
				mConnected = false;
			}
		}
	}
}

void Game::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			// for now mouse button is creating a pencil object, but what gets created should change based on selection, maybe a ObjectManager
			mActiveGameObjectList.push_back(new PencilObject(Vector2(event.motion.x, event.motion.y)));
			if (mConnected)
			{
				charbuf buf;
				sprintf_s(buf, "Mouse clicked at: (%d, %d)\n", event.motion.x, event.motion.y);
				mMsg.LoadBytes(buf, 256);
				mMsg.Finish();
				mTCPClient->Send(mMsg);
			}
			break;
		case SDL_KEYDOWN:
			keysPressed[event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			keysPressed[event.key.keysym.scancode] = false;
			break;

		default:
			break;
		}

		// should maintain separate input event manager that dispatches input events that objects can listen for if they still care about input
		// for now forwarding event data to most recent object
		if (mActiveGameObjectList.size() > 0)
		{
			mActiveGameObjectList[mActiveGameObjectList.size() - 1]->handleInput(event);
		}
	}

	// undo implementation by popping the last object from the stack
	if (keysPressed[SDL_SCANCODE_LCTRL] && keysPressed[SDL_SCANCODE_Z])
	{
		if (mActiveGameObjectList.size() > 0)
		{
			delete mActiveGameObjectList.back();
			mActiveGameObjectList.pop_back();
		}
		keysPressed[SDL_SCANCODE_Z] = false;
	}	
}

void Game::update()
{
	handleNetworkData();

	Uint32 currentFrameTime = SDL_GetTicks();
	float deltaTime = (currentFrameTime - mLastFrameTime)/1000.0f;

	for (size_t i = 0; i < mActiveGameObjectList.size(); i++)
	{
		if (mActiveGameObjectList[i] != NULL)
		{
			mActiveGameObjectList[i]->update(deltaTime);
		}
	}

	mLastFrameTime = currentFrameTime;
}

void Game::render()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	for (size_t i = 0; i < mActiveGameObjectList.size(); i++)
	{
		mActiveGameObjectList[i]->render();
	}

	SDL_GL_SwapWindow(mWindow);
}

void Game::clean()
{
	for (size_t i = 0; i < mActiveGameObjectList.size(); i++)
	{
		delete mActiveGameObjectList[i];
	}

	delete mTCPListener;
	delete mTCPClient;
	delete mRemoteIP;

	NetworkManager::Quit();
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	std::cout << "Game cleaned" << std::endl;
}

