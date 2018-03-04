#include "NetworkManager.h"

#define SDLNET_ERROR(s) std::cerr << s << ": " << SDLNet_GetError() << std::endl

//===================================================================================================================

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
	SDLNet_Quit();
}


//===================================================================================================================

NetworkMessage::NetworkMessage()
{
	Reset();
}

int NetworkMessage::NumToLoad()
{
	if (mState == EMPTY)
	{
		return 255;
	}
	return 0;
}

int NetworkMessage::NumToUnload()
{
	if (mState == FULL)
	{
		return 4;// strlen(mBuffer) + 1;
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
}

void NetworkMessage::UnLoadBytes(charbuf & destBuffer)
{
	for (int i = 0; i < this->NumToUnload(); i++)
	{
		destBuffer[i] = mBuffer[i];
	}
	Reset();
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
	for (int i = 0; i < 256; i++)
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
	mSocket = NULL;
	mSet = SDLNet_AllocSocketSet(1);
}

TCPSocketBase::~TCPSocketBase()
{
	if (mSocket != NULL)
	{
		SDLNet_TCP_DelSocket(mSet, mSocket);
		SDLNet_FreeSocketSet(mSet);
		SDLNet_TCP_Close(mSocket);
	}
}

void TCPSocketBase::SetSocket(TCPsocket socket)
{
	if (mSocket != NULL)
	{
		SDLNet_TCP_DelSocket(mSet, mSocket); 
		SDLNet_TCP_Close(mSocket);
	}

	mSocket = socket;

	if (SDLNet_TCP_AddSocket(mSet, mSocket) == -1)
	{
		SDLNET_ERROR("SDLNet_TCP_AddSocket");
		mSocket = NULL;
	}
}

bool TCPSocketBase::Ready() const
{
	bool ready = false;
	int numReady = SDLNet_CheckSockets(mSet, 0);
	if (numReady == -1)
	{
		SDLNET_ERROR("SDLNet_CheckSockets");
	}
	else if (numReady > 0)
	{
		ready = SDLNet_SocketReady(mSocket);
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

	mSocket = SDLNet_TCP_Open(&ip);
	if (mSocket == NULL)
	{
		SDLNet_FreeSocketSet(mSet);
		SDLNET_ERROR("SDLNet_TCP_Open");
	}
}

HostSocketTCP::HostSocketTCP(Uint16 port)
{
	ConnectionInfo listenerInfo(port);
	if (!listenerInfo.Valid())
	{
		mSocket = NULL;
	}
	else
	{
		TCPSocketBase::TCPSocketBase();
		IPaddress ip = listenerInfo.GetIP();
		mSocket = SDLNet_TCP_Open(&ip);
		if (mSocket == NULL)
		{
			SDLNet_FreeSocketSet(mSet);
			SDLNET_ERROR("SDLNet_TCP_Open");
		}
	}
}

bool HostSocketTCP::Accept(ClientSocketTCP &clientSocket)
{
	TCPsocket sock = SDLNet_TCP_Accept(mSocket);
	if (sock != NULL)
	{
		clientSocket.SetSocket(sock);
		return true;
	}
	return false;
}

void HostSocketTCP::OnReady()
{
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
		mSocket = NULL;
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
	IPaddress * ips = SDLNet_TCP_GetPeerAddress(mSocket);
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

bool ClientSocketTCP::Recieve(NetworkMessage & data)
{
	if (mSocket == NULL) return false;

	charbuf buf;

	while (data.NumToLoad() > 0)
	{
		if (SDLNet_TCP_Recv(mSocket, buf, data.NumToLoad()) > 0)
		{
			data.LoadBytes(buf, data.NumToLoad());
		}
		else
		{
			return false;
		}
	}

	data.Finish();

	return true;
}

bool ClientSocketTCP::Send(NetworkMessage & data)
{
	if (mSocket == NULL) return false;

	charbuf buf;
	int len;

	while ((len = data.NumToUnload()) > 0)
	{
		data.UnLoadBytes(buf);
		if (SDLNet_TCP_Send(mSocket, (void*)buf, len) < len)
		{
			SDLNET_ERROR("SDLNet_TCP_Send");
			return false;
		}
	}

	return true;
}

void ClientSocketTCP::OnReady()
{
}

//===================================================================================================================