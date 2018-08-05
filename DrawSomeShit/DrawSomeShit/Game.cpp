#include "Game.h"

#define MAX_TEXT_LINES 10

Game * Game::sInstance = NULL;

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

	mPlayerID = -1;
	mTestFontSheet = NULL;
	mCurrentWord = NULL;
	mCurrentPlayerIndex = -1;
	mCurrentDrawerPlayerID = 0;
	mNextWord = "";

	mStateMachine = new StateMachine<Game>(this);
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
	mScreenWidth = width;
	mScreenHeight = height;

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

	SDL_StartTextInput();
	setupOpenGL(width, height);
	mLastFrameTime = SDL_GetTicks();
	loadMedia();

	mGameStates[GameState::Reset] = new ResetGameState(mIsServer);
	mGameStates[GameState::SelectWord] = new SelectWordGameState(mIsServer);
	mGameStates[GameState::Drawing] = new DrawingGameState(mIsServer);
	mStateMachine->ChangeState(mGameStates[GameState::Reset]);
	mCurrentGameStateID = GameState::Reset;
}

void Game::ChangeGameStateServer(GameState state)
{
	if (NetworkManager::IsConnected() && mIsServer)
	{
		charbuf buf;
		int offset = 0;

		int idPackedData = 0;
		idPackedData |= mPlayerID;
		//int id = obj->GetID();
		memcpy(buf + offset, &idPackedData, sizeof(int));
		offset += sizeof(int);

		int packedData = 0;	// 32 bit data
		packedData |= ObjectNetworkMessageType::GAMESTATECHANGE; // 0-7 bits command/id
		packedData |= state << 8;
		memcpy(buf + offset, &packedData, sizeof(int));
		NetworkManager::SendNetworkMessage(buf, 8);

		ChangeGameStateLocal(state);
	}
}

void Game::ChangeGameStateLocal(GameState state)
{
	mStateMachine->ChangeState(mGameStates[state]);
	mCurrentGameStateID = state;
}

void Game::SetCurrentWord(std::string str)
{
	mCurrentWord->setText(str);
}

void Game::OnNewPlayerConnected(int id)
{
	mPlayerOrder.push_back(id);
}

void Game::UpdateNextDrawer()
{
	mCurrentPlayerIndex++;
	if (mCurrentPlayerIndex >= mPlayerOrder.size())
	{
		mCurrentPlayerIndex = 0;
	}

	if (NetworkManager::IsConnected() && mIsServer)
	{
		charbuf buf;
		int offset = 0;

		int idPackedData = 0;
		idPackedData |= mPlayerID;
		//int id = obj->GetID();
		memcpy(buf + offset, &idPackedData, sizeof(int));
		offset += sizeof(int);

		int packedData = 0;	// 32 bit data
		packedData |= ObjectNetworkMessageType::NEXTDRAWERID; // 0-7 bits command/id
		packedData |= mPlayerOrder[mCurrentPlayerIndex] << 8;
		memcpy(buf + offset, &packedData, sizeof(int));
		NetworkManager::SendNetworkMessage(buf, 8);

		mCurrentDrawerPlayerID = mPlayerOrder[mCurrentPlayerIndex];
	}
}

void Game::UpdateNextWord()
{
	if (NetworkManager::IsConnected() && mIsServer)
	{
		std::string word = mWordlist[rand() % mWordlist.size()];

		charbuf buf;
		int offset = 0;
		int idPackedData = 0;
		idPackedData |= mPlayerID;
		memcpy(buf + offset, &idPackedData, sizeof(int));
		offset += sizeof(int);

		int packedData = 0;
		packedData |= ObjectNetworkMessageType::NEXTWORD;
		packedData |= word.length() << 8;
		memcpy(buf + offset, &packedData, sizeof(int));
		offset += sizeof(int);

		memcpy(buf + offset, word.c_str(), word.length());

		NetworkManager::SendNetworkMessage(buf, 8 + word.length());

		mNextWord = word;
	}
}

void Game::setupOpenGL(int width, int height)
{
	// initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
		return;
	}

	if (!GLEW_VERSION_2_1)
	{
		std::cout << "OpenGL 2.1 not supported!" << std::endl;
		return;
	}

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

	//Set blending
	//glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0.0);

	// initialize DevIL
	ilInit();
	ilClearColor(255, 255, 255, 000);
}

void Game::loadMedia()
{
	mTestFontSheet = new FontSheet();

	if (!mTestFontSheet->loadBitmap("Assets/lazy_font.png"))
	{
		std::cout << "Failed to load font sheet!" << std::endl;
	}

	std::ifstream myFile("Assets/easy.txt");
	std::string line;
	if (myFile)
	{
		while (std::getline(myFile, line))
		{
			mWordlist.push_back(line);
		}
		myFile.close();
	}

	if (NetworkManager::mConnectionType == ConnectionType::HOST)
	{
		std::stringstream ss;
		ss << mPlayerID << ": ";
		mNextTextObject = new TextObject(Vector2(100, mScreenHeight - 100), Utils::GetNextObjectIDForPlayer(mPlayerID), mPlayerID, mTestFontSheet, ss.str());
		mPlayerOrder.push_back(mPlayerID);
	}

	// std::string word = mWordlist[rand() % mWordlist.size()];
	mCurrentWord = new TextObject(Vector2(mScreenWidth / 2.0f, 100), Utils::GetNextObjectIDForPlayer(mPlayerID), mPlayerID, mTestFontSheet, "");
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
			NetworkManager::mConnectionType = ConnectionType::HOST;
			break;
		case 2:
			std::cout << "Enter the host ip: ";
			std::cin >> ipString;
			NetworkManager::mConnectionType = ConnectionType::CLIENT;
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

	NetworkManager::StartConnection(ipString);
	if (NetworkManager::mConnectionType == ConnectionType::HOST)
	{
		mPlayerID = 0;
		mIsServer = true;
	}
	else
	{
		mIsServer = false;
	}
}

void Game::processCommandBuffer(charbuf & buf, int bufSize)
{
	if (bufSize == 0) return; // nothing to process

	// if player id is not yet set don't do accept any other data
	if (mPlayerID == -1)
	{
		memcpy(&mPlayerID, buf, sizeof(int));
		std::cout << "Player ID set to " << mPlayerID << std::endl;
		std::stringstream ss;
		ss << mPlayerID << ": ";
		mNextTextObject = new TextObject(Vector2(100, mScreenHeight - 100), Utils::GetNextObjectIDForPlayer(mPlayerID), mPlayerID, mTestFontSheet, ss.str());
		return;
	}

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
		int textLength = (packedData & 0xFFFFFF00) >> 8;
		offset += sizeof(int);

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
			case TEXT:
			{
				char textRec[128];
				memcpy(textRec, buf + offset, textLength);
				textRec[textLength] = '\0';
				offset += textLength;
				TextObject * mNewText = new TextObject(Vector2(100, mScreenHeight - 100), id, playerID, mTestFontSheet, textRec);
				mNewText->setSent(true);
				mTextObjects.push_back(mNewText);
				for (int i = 0; i < mTextObjects.size(); i++)
				{
					((TextObject*)mTextObjects[i])->forceUp();
				}
				mNewText = NULL;

				if (mTextObjects.size() > MAX_TEXT_LINES)
				{
					delete mTextObjects.front();
					mTextObjects.erase(mTextObjects.begin());
				}

				std::string textGuess = textRec;
				if (mCurrentGameStateID == GameState::Drawing && mIsServer)
				{
					((DrawingGameState*)mStateMachine->GetCurrentState())->RecieveGuess(textGuess, playerID);
				}

				break;
			}
			case GAMESTATECHANGE:
				ChangeGameStateLocal((GameState)x);
				break;
			case NEXTDRAWERID:
				mCurrentDrawerPlayerID = x;
				break;
			case NEXTWORD:
			{
				char textRec[128];
				memcpy(textRec, buf + offset, textLength);
				textRec[textLength] = '\0';
				offset += textLength;
				mNextWord = textRec;
				break;
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
		{
			// for now mouse button is creating a pencil object, but what gets created should change based on selection, maybe a ObjectManager
			PencilObject * obj = new PencilObject(Vector2((float)event.motion.x, (float)event.motion.y), Utils::GetNextObjectIDForPlayer(mPlayerID), mPlayerID);
			mActiveGameObjectList.push_back(obj);
			if (NetworkManager::IsConnected())
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
				NetworkManager::SendNetworkMessage(buf, 8);
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
		for (int i = 0; i < mTextObjects.size(); i++)
		{
			mTextObjects[i]->handleInput(event);
		}
		if (mNextTextObject != NULL)
		{
			mNextTextObject->handleInput(event);
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
			NetworkManager::SendNetworkMessage(buf, 8);

			delete mActiveGameObjectList.back();
			mActiveGameObjectList.pop_back();
		}
		keysPressed[SDL_SCANCODE_Z] = false;
	}

	// typing
	if (keysPressed[SDL_SCANCODE_RETURN] && !mEnterWasPressed)
	{
		mEnterWasPressed = true;
		std::stringstream ss;
		ss << mPlayerID << ": ";
		mTextObjects.push_back(mNextTextObject);
		std::string textGuess = mNextTextObject->getText();
		if (mCurrentGameStateID == GameState::Drawing && mIsServer)
		{
			((DrawingGameState*)mStateMachine->GetCurrentState())->RecieveGuess(textGuess, 0);
		}

		mNextTextObject = new TextObject(Vector2(100, mScreenHeight - 100), Utils::GetNextObjectIDForPlayer(mPlayerID), mPlayerID, mTestFontSheet, ss.str());

		if (mTextObjects.size() > MAX_TEXT_LINES)
		{
			delete mTextObjects.front();
			mTextObjects.erase(mTextObjects.begin());
		}
	}
	else if (!keysPressed[SDL_SCANCODE_RETURN] && mEnterWasPressed)
	{
		mEnterWasPressed = false;
	}
}

void Game::update()
{
	charbuf buf;
	int bufSize = NetworkManager::HandleNetworkData(buf);
	processCommandBuffer(buf, bufSize);

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

	mStateMachine->Update(deltaTime);

	mLastFrameTime = currentFrameTime;
}

void Game::render()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	for (size_t i = 0; i < mTextObjects.size(); i++)
	{
		if (mTextObjects[i] != NULL)
		{
			mTextObjects[i]->render();
		}
	}
	if (mNextTextObject != NULL)
	{
		mNextTextObject->render();
	}
	if (mCurrentWord != NULL)
	{
		mCurrentWord->render();
	}

	SDL_GL_SwapWindow(mWindow);
}

void Game::clean()
{
	delete mTestFontSheet;

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

	for (size_t i = 0; i < mTextObjects.size(); i++)
	{
		if (mTextObjects[i] != NULL)
		{
			delete mTextObjects[i];
		}
	}

	delete mCurrentWord;

	for (std::map<GameState, State<Game>*>::iterator it = mGameStates.begin(); it != mGameStates.end(); ++it)
	{
		delete it->second;
	}

	delete mStateMachine;
	
	NetworkManager::Quit();
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	std::cout << "Game cleaned" << std::endl;
}

