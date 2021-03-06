#pragma once
#include "SDL_net.h"
#include <iostream>

typedef char charbuf[1024];
#define MAX_SOCKETS 8

enum ConnectionType
{
	NONE,
	SERVER,
	CLIENT,
	HOST
};

// forward decs
class ClientSocketTCP;
class ConnectionInfo;
class NetworkMessage;
class HostSocketTCP;

class NetworkManager
{
public:
	static bool Init(); // Initialize SDL_net
	static void Quit(); // Exit SDL_net
	static void StartConnection(std::string ipString);
	static int HandleNetworkData(charbuf & outVal);
	static void SendNetworkMessage(charbuf & dataBuf, int bufSize, int clientIndex = -1);
	static bool IsConnected() { return mConnected; }

public:
	static ConnectionType mConnectionType;

private:
	// networking variables
	static bool mConnected;
	static ClientSocketTCP * mTCPClient;
	static ConnectionInfo * mRemoteIP;
	static NetworkMessage mMsg;
	static HostSocketTCP * mTCPListener;
};

class NetworkMessage
{
public:
	NetworkMessage();

	virtual int NumToLoad();

	virtual int NumToUnload();

	void LoadBytes(charbuf& inputBuffer, int n);

	int UnLoadBytes(charbuf& destBuffer);

	void Finish();

protected:
	
	void Reset();
	
	charbuf mBuffer; // message stored in buffer, limited to 256 bytes

	enum BufferStates
	{
		EMPTY,
		READING,
		WRITING,
		FULL
	};
	BufferStates mState;
	int mBufSize;
};

class ConnectionInfo
{
public:
	ConnectionInfo();
	ConnectionInfo(Uint16 port);
	ConnectionInfo(const char * host, Uint16 port);

	bool Valid() const;

	void SetIP(IPaddress sdl_IP) { mIP = sdl_IP; }

	IPaddress GetIP() const { return mIP; }
	Uint32 GetHost() const { return mIP.host; }
	Uint16 GetPort() const { return mIP.port; }
private:
	IPaddress mIP;
};

class TCPSocketBase
{
public:
	TCPSocketBase();
	virtual ~TCPSocketBase();

	virtual void SetSocket(TCPsocket socket);
	bool Valid() const { return true; }
	bool Ready() const;

	virtual void OnReady();

protected:
	TCPsocket mSockets[MAX_SOCKETS];
	SDLNet_SocketSet mSet;
};

class ClientSocketTCP;

class HostSocketTCP : public TCPSocketBase
{
public:
	HostSocketTCP(ConnectionInfo *addressInfo);
	HostSocketTCP(Uint16 port);

	bool Accept(ClientSocketTCP&);
	virtual void OnReady();
	int GetNextPlayerID();

protected:
	TCPsocket mServerSocket;
	int mPlayerIDCounter;
};

class ClientSocketTCP : public TCPSocketBase
{
public:
	ClientSocketTCP();
	ClientSocketTCP(const char * host, Uint16 port);
	
	bool Connect(ConnectionInfo &addressInfo);
	bool Connect(HostSocketTCP& listenerSocket);

	void SetSocket(TCPsocket socket);
	ConnectionInfo GetConnectionInfo() { return mRemoteConnectionInfo; }

	bool Recieve(NetworkMessage &data, int index);
	bool Send(NetworkMessage &data, int indexToIgnore = -1);

	virtual void OnReady();
	void SetPlayerID(int id) { mPlayerID = id; }
	int GetPlayerID() { return mPlayerID; }

private:
	ConnectionInfo mRemoteConnectionInfo;
	int mPlayerID;
};
