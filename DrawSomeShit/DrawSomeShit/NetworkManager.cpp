#include "NetworkManager.h"
#include "Game.h"

#define SDLNET_ERROR(s) std::cerr << s << ": " << SDLNet_GetError() << std::endl
#define PORT 6969

//===================================================================================================================

ConnectionType NetworkManager::mConnectionType = ConnectionType::NONE;
HostSocketTCP * NetworkManager::mTCPListener = NULL;
ClientSocketTCP * NetworkManager::mTCPClient = NULL;
bool NetworkManager::mConnected = false;
ConnectionInfo * NetworkManager::mRemoteIP = NULL;
NetworkMessage NetworkManager::mMsg;

bool NetworkManager::Init()
{
	if (SDLNet_Init() < 0)
	{
		SDLNET_ERROR("SDLNet_Init");
		return false;
	}
	return true;
}

void NetworkManager::Quit()
{
	delete mTCPListener;
	delete mTCPClient;
	delete mRemoteIP;

	SDLNet_Quit();
}

void NetworkManager::StartConnection(std::string ipString)
{
	if (mConnectionType == ConnectionType::HOST)
	{
		mTCPListener = new HostSocketTCP(PORT);
		if (!mTCPListener->Valid())
		{
			exit(EXIT_FAILURE);
		}
		mTCPClient = new ClientSocketTCP();
		mTCPClient->SetPlayerID(0);
	}
	else if (mConnectionType == ConnectionType::CLIENT)
	{
		mTCPClient = new ClientSocketTCP();

		mRemoteIP = new ConnectionInfo(ipString.c_str(), PORT);
	}
}

int NetworkManager::HandleNetworkData(charbuf & outVal)
{
	int bufSize = 0;

	if (!mConnected)
	{
		if (NetworkManager::mConnectionType == ConnectionType::HOST)
		{
			if (mTCPListener->Accept(*mTCPClient))
			{
				mConnected = true;
			}
		}
		else if (NetworkManager::mConnectionType == ConnectionType::CLIENT)
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
		if (NetworkManager::mConnectionType == ConnectionType::HOST)
		{
			if (mTCPListener->Accept(*mTCPClient))
			{
				mConnected = true;
			}
		}

		if (mTCPClient->Ready())
		{
			if (NetworkManager::mConnectionType == ConnectionType::HOST)
			{
				for (int i = 0; i < MAX_SOCKETS; i++)
				{
					if (mTCPClient->Recieve(mMsg, i))
					{
						bufSize = mMsg.UnLoadBytes(outVal);
						SendNetworkMessage(outVal, bufSize, i);
					}
					else
					{
						//mConnected = false;
					}
				}
			}
			else if (NetworkManager::mConnectionType == ConnectionType::CLIENT)
			{
				if (mTCPClient->Recieve(mMsg, 0))
				{
					bufSize = mMsg.UnLoadBytes(outVal);
				}
				else
				{
					//mConnected = false;
				}
			}
		}
	}

	return bufSize;
}

void NetworkManager::SendNetworkMessage(charbuf & dataBuf, int bufSize, int clientIndex)
{
	mMsg.LoadBytes(dataBuf, bufSize);
	mMsg.Finish();
	mTCPClient->Send(mMsg, clientIndex);
}


//===================================================================================================================

NetworkMessage::NetworkMessage()
{
	mBufSize = 0;
	Reset();
}

int NetworkMessage::NumToLoad()
{
	if (mState == EMPTY)
	{
		return 1024;
	}
	return 0;
}

int NetworkMessage::NumToUnload()
{
	if (mState == FULL)
	{
		return mBufSize;// strlen(mBuffer) + 1;
	}
	return 0;
}

void NetworkMessage::LoadBytes(charbuf & inputBuffer, int n)
{
	for (int i = 0; i < n; i++)
	{
		mBuffer[i] = inputBuffer[i];
	}
	mState = READING;
	mBufSize = n;
}

int NetworkMessage::UnLoadBytes(charbuf & destBuffer)
{
	int bufSize = this->NumToUnload();

	for (int i = 0; i < this->NumToUnload(); i++)
	{
		destBuffer[i] = mBuffer[i];
	}
	Reset();

	return bufSize;
}

void NetworkMessage::Finish()
{
	if (mState == READING)
	{
		mState = FULL;
	}
}

void NetworkMessage::Reset()
{
	for (int i = 0; i < 1024; i++)
	{
		mBuffer[i] = 0;
	}

	mState = EMPTY;
}

//===================================================================================================================

ConnectionInfo::ConnectionInfo()
{
	mIP.host = 0;
	mIP.port = 0;
}

ConnectionInfo::ConnectionInfo(Uint16 port)
{
	if (SDLNet_ResolveHost(&mIP, NULL, port) < 0)
	{
		SDLNET_ERROR("SDLNet_ResolveHost");
		mIP.host = 0;
		mIP.port = 0;
	}
}

ConnectionInfo::ConnectionInfo(const char * host, Uint16 port)
{
	if (SDLNet_ResolveHost(&mIP, host, port) < 0)
	{
		SDLNET_ERROR("SDLNet_ResolveHost");
		mIP.host = 0;
		mIP.port = 0;
	}
}

bool ConnectionInfo::Valid() const
{
	return !(mIP.port == 0);
}

//===================================================================================================================

TCPSocketBase::TCPSocketBase()
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		mSockets[i] = NULL;
	}
	mSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
}

TCPSocketBase::~TCPSocketBase()
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (mSockets[i] != NULL)
		{
			SDLNet_TCP_DelSocket(mSet, mSockets[i]);
			SDLNet_TCP_Close(mSockets[i]);
		}
	}
	SDLNet_FreeSocketSet(mSet);
}

void TCPSocketBase::SetSocket(TCPsocket socket)
{
	int socketIndex = 0;
	// find next available socket
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (mSockets[i] == NULL)
		{
			socketIndex = i;
			break;
		}
	}

	mSockets[socketIndex] = socket;

	if (SDLNet_TCP_AddSocket(mSet, mSockets[socketIndex]) == -1)
	{
		SDLNET_ERROR("SDLNet_TCP_AddSocket");
		mSockets[socketIndex] = NULL;
	}
}

bool TCPSocketBase::Ready() const
{
	bool ready = false;
	int numReady = SDLNet_CheckSockets(mSet, 0);
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (numReady == -1)
		{
			SDLNET_ERROR("SDLNet_CheckSockets");
		}
		else if (numReady > 0)
		{
			ready = true;  //SDLNet_SocketReady(mSockets[i]);
			break;
		}
	}
	return ready;
}

void TCPSocketBase::OnReady()
{
}

//===================================================================================================================

HostSocketTCP::HostSocketTCP(ConnectionInfo * addressInfo)
{
	TCPSocketBase::TCPSocketBase();
	IPaddress ip = addressInfo->GetIP();

	mServerSocket = SDLNet_TCP_Open(&ip);
	if (mServerSocket == NULL)
	{
		SDLNet_FreeSocketSet(mSet);
		SDLNET_ERROR("SDLNet_TCP_Open");
	}

	mPlayerIDCounter = 0;
}

HostSocketTCP::HostSocketTCP(Uint16 port)
{
	ConnectionInfo listenerInfo(port);
	if (!listenerInfo.Valid())
	{
		mServerSocket = NULL;
	}
	else
	{
		TCPSocketBase::TCPSocketBase();
		IPaddress ip = listenerInfo.GetIP();
		mServerSocket = SDLNet_TCP_Open(&ip);
		if (mServerSocket == NULL)
		{
			SDLNet_FreeSocketSet(mSet);
			SDLNET_ERROR("SDLNet_TCP_Open");
		}
	}
}

bool HostSocketTCP::Accept(ClientSocketTCP &clientSocket)
{
	TCPsocket sock = SDLNet_TCP_Accept(mServerSocket);
	if (sock != NULL)
	{
		clientSocket.SetSocket(sock);
		int nextID = GetNextPlayerID();
		SDLNet_TCP_Send(sock, &nextID, sizeof(int));
		Game::Instance()->OnNewPlayerConnected(nextID);
		return true;
	}
	return false;
}

void HostSocketTCP::OnReady()
{
}

int HostSocketTCP::GetNextPlayerID()
{
	mPlayerIDCounter++;
	return mPlayerIDCounter;
}

//===================================================================================================================

ClientSocketTCP::ClientSocketTCP()
{
	TCPSocketBase::TCPSocketBase();
}

ClientSocketTCP::ClientSocketTCP(const char * host, Uint16 port)
{
	ConnectionInfo remoteInfo(host, port);
	if (!remoteInfo.Valid())
	{
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			mSockets[i] = NULL;
		}
	}
	else
	{
		TCPSocketBase::TCPSocketBase();
		Connect(remoteInfo);
	}
}

bool ClientSocketTCP::Connect(ConnectionInfo & addressInfo)
{
	TCPsocket socket;
	IPaddress ip = addressInfo.GetIP();

	socket = SDLNet_TCP_Open(&ip);
	if (socket != NULL)
	{
		SetSocket(socket);
		return true;
	}

	SDLNET_ERROR("SDLNet_TCP_Open");
	return false;
}

bool ClientSocketTCP::Connect(HostSocketTCP & listenerSocket)
{
	return false;
}

void ClientSocketTCP::SetSocket(TCPsocket socket)
{
	TCPSocketBase::SetSocket(socket);
	IPaddress * ips = SDLNet_TCP_GetPeerAddress(socket);
	if (ips != NULL)
	{
		mRemoteConnectionInfo.SetIP(*ips);
		Uint32 host = mRemoteConnectionInfo.GetHost();
		Uint16 port = mRemoteConnectionInfo.GetPort();
		std::cout << "Client connected: " << SDLNet_Read32(&host) << " " << SDLNet_Read16(&port) << std::endl;
	}
	else
	{
		SDLNET_ERROR("SDLNet_TCP_GetPeerAddress");
	}
}

bool ClientSocketTCP::Recieve(NetworkMessage & data, int index)
{
	if (SDLNet_SocketReady(mSockets[index]))
	{
		charbuf buf;
		while (data.NumToLoad() > 0)
		{
			int numBytes = SDLNet_TCP_Recv(mSockets[index], buf, data.NumToLoad());
			if (numBytes > 0)
			{
				data.LoadBytes(buf, numBytes);
			}
			else
			{
				return false;
			}
		}
		data.Finish();

		return true;
	}
	else
	{
		return false;
	}
}

bool ClientSocketTCP::Send(NetworkMessage & data, int indexToIgnore)
{
	charbuf buf;
	int len;

	while ((len = data.NumToUnload()) > 0)
	{
		data.UnLoadBytes(buf);

		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (mSockets[i] == NULL || i == indexToIgnore) continue;

			if (SDLNet_TCP_Send(mSockets[i], (void*)buf, len) < len)
			{
				SDLNET_ERROR("SDLNet_TCP_Send");
				return false;
			}
		}
	}

	return true;
}

void ClientSocketTCP::OnReady()
{
}

//===================================================================================================================