#include "Game.h"

#define PORT 6969
#define MAX_OBJECTS 10000

Game * Game::sInstance = NULL;
Rect gTestClipRect;

Game::Game() : mIsRunning(false), mLastFrameTime(0), mIsMouseDown(false)
{
	if (sInstance == NULL)
	{
		sInstance = this;
	}
	else
	{
		std::cerr << "Got a 2nd instance of game somehow. This shouldn't happen.";
	}

	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		keysPressed[i] = false;
	}

	mTCPListener = NULL;
	mTCPClient = NULL;
	mConnected = false;
	mRemoteIP = NULL;
	mConnectionType = ConnectionType::NONE;
	mPlayerID = -1;
	mObjectIDCounter = 0;
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
	loadMedia();
}

void Game::SendNetworkMessage(charbuf & dataBuf, int msgSize)
{
	mMsg.LoadBytes(dataBuf, msgSize);
	mMsg.Finish();
	mTCPClient->Send(mMsg);
}

void Game::ForwardMessageToClients(charbuf & dataBuf, int clientIndex, int bufSize)
{
	mMsg.LoadBytes(dataBuf, bufSize);
	mMsg.Finish();
	mTCPClient->Send(mMsg, clientIndex);
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

	// enable texturing
	glEnable(GL_TEXTURE_2D);

	// initialize DevIL
	ilInit();
	ilClearColor(255, 255, 255, 000);
}

void Game::loadMedia()
{
	gTestClipRect.x = 0.0f;
	gTestClipRect.y = 128.0f;
	gTestClipRect.w = 128.0f;
	gTestClipRect.h = 128.0f;

	mTestObject = new TextureObject(Vector2(200, 200), &gTestClipRect, -1, -1);

	//Load texture
	if (!mTestObject->loadTextureFromFile("Assets/texture.png"))
	{
		printf("Unable to load checkerboard texture!\n");
	}
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
		mTCPClient->SetPlayerID(0);
		mPlayerID = 0;
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
		if (mConnectionType == ConnectionType::HOST)
		{
			if (mTCPListener->Accept(*mTCPClient))
			{
				mConnected = true;
			}
		}

		if (mTCPClient->Ready())
		{
			if (mPlayerID == -1)
			{
				if (mTCPClient->Recieve(mMsg, 0))
				{
					char buf[1024];
					mMsg.UnLoadBytes(buf);
					memcpy(&mPlayerID, buf, sizeof(int));
					std::cout << "Player ID set to " << mPlayerID << std::endl;
				}
			}
			else
			{
				if (mConnectionType == ConnectionType::HOST)
				{
					for (int i = 0; i < MAX_SOCKETS; i++)
					{
						if (mTCPClient->Recieve(mMsg, i))
						{
							char buf[1024];
							int bufSize = mMsg.UnLoadBytes(buf);
							ForwardMessageToClients(buf, i, bufSize);
							processCommandBuffer(buf, bufSize);
						}
						else
						{
							//mConnected = false;
						}
					}
				}
				else if (mConnectionType == ConnectionType::CLIENT)
				{
					if (mTCPClient->Recieve(mMsg, 0))
					{
						char buf[1024];
						int bufSize = mMsg.UnLoadBytes(buf);
						processCommandBuffer(buf, bufSize);
					}
					else
					{
						//mConnected = false;
					}
				}
			}			
		}
	}
}

void Game::processCommandBuffer(charbuf buf, int bufSize)
{
	int offset = 0;
	while (offset < bufSize)
	{
		int idPackedData;
		memcpy(&idPackedData, buf + offset, sizeof(int));
		int playerID = (idPackedData & 0x000000FF);
		int id = (idPackedData & 0xFFFFFF00) >> 8;
		offset += sizeof(int);

		int packedData;
		memcpy(&packedData, buf + offset, sizeof(int));
		ObjectNetworkMessageType type2 = (ObjectNetworkMessageType)(packedData & 0x000000FF);
		int x = (packedData & 0x000FFF00) >> 8;
		int y = (packedData & 0xFFF00000) >> 20;

		switch (type2)
		{
		case CREATE:
		{
			PencilObject * obj = new PencilObject(Vector2((float)x, (float)y), id, playerID);
			std::cout << "Created object id " << obj->GetID() << std::endl;
			mNetworkedGameObjectList.push_back(obj);
			mNetworkedGameObjectMap[obj->GetID()] = obj;
			break;
		}
		case UPDATE:
		case FINISH:
			if (mNetworkedGameObjectList.size() > 0)
			{
				//mNetworkedGameObjectList[mNetworkedGameObjectList.size() - 1]->HandleNetworkData(packedData);
				if (mNetworkedGameObjectMap[id] != NULL)
				{
					mNetworkedGameObjectMap[id]->HandleNetworkData(packedData);
				}
			}
			break;
		case REMOVE:
			if (mNetworkedGameObjectMap[id] != NULL)
			{
				for (std::vector<GameObject *>::iterator it = mNetworkedGameObjectList.begin(); it != mNetworkedGameObjectList.end(); ++it)
				{
					if ((*it)->GetID() == id)
					{
						mNetworkedGameObjectList.erase(it);
						break;
					}
				}
				mNetworkedGameObjectMap.erase(id);
				delete mNetworkedGameObjectMap[id];
			}
			break;
		}

		offset += sizeof(int);
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
		{
			// for now mouse button is creating a pencil object, but what gets created should change based on selection, maybe a ObjectManager
			PencilObject * obj = new PencilObject(Vector2((float)event.motion.x, (float)event.motion.y), mPlayerID * MAX_OBJECTS + mObjectIDCounter, mPlayerID);
			mActiveGameObjectList.push_back(obj);
			mObjectIDCounter++;
			if (mConnected)
			{
				charbuf buf;
				int offset = 0;

				int idPackedData = 0;
				idPackedData |= mPlayerID;
				idPackedData |= obj->GetID() << 8;
				//int id = obj->GetID();
				memcpy(buf + offset, &idPackedData, sizeof(int));
				offset += sizeof(int);

				int packedData = 0;	// 32 bit data
				packedData |= ObjectNetworkMessageType::CREATE; // 0-7 bits command/id
				packedData |= event.motion.x << 8; 				// 8-19 bits x
				packedData |= event.motion.y << 20;				// 20-31 bits y
				memcpy(buf + offset, &packedData, sizeof(int));
				SendNetworkMessage(buf, 8);
			}
			break;
		}
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
			charbuf buf;
			int offset = 0;

//			int id = mActiveGameObjectList.back()->GetID();
			int idPackedData = 0;
			idPackedData |= mPlayerID;
			idPackedData |= mActiveGameObjectList.back()->GetID() << 8;

			memcpy(buf + offset, &idPackedData, sizeof(int));
			offset += sizeof(int);

			int packedData = 0;
			packedData |= ObjectNetworkMessageType::REMOVE;
			memcpy(buf + offset, &packedData, sizeof(int));
			Game::Instance()->SendNetworkMessage(buf, 8);

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

	for (size_t i = 0; i < mNetworkedGameObjectList.size(); i++)
	{
		if (mNetworkedGameObjectList[i] != NULL)
		{
			mNetworkedGameObjectList[i]->update(deltaTime);
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
		if (mActiveGameObjectList[i] != NULL)
		{
			mActiveGameObjectList[i]->render();
		}
	}

	for (size_t i = 0; i < mNetworkedGameObjectList.size(); i++)
	{
		if (mNetworkedGameObjectList[i] != NULL)
		{
			mNetworkedGameObjectList[i]->render();
		}
	}

	mTestObject->render();

	SDL_GL_SwapWindow(mWindow);
}

void Game::clean()
{
	delete mTestObject;

	for (size_t i = 0; i < mActiveGameObjectList.size(); i++)
	{
		if (mActiveGameObjectList[i] != NULL)
		{
			delete mActiveGameObjectList[i];
		}
	}

	for (size_t i = 0; i < mNetworkedGameObjectList.size(); i++)
	{
		if (mNetworkedGameObjectList[i] != NULL)
		{
			delete mNetworkedGameObjectList[i];
		}
	}
	
	delete mTCPListener;
	delete mTCPClient;
	delete mRemoteIP;

	NetworkManager::Quit();
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	std::cout << "Game cleaned" << std::endl;
}

