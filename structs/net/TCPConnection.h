#pragma once

#include <string>

#include <SDL2/SDL_net.h>

class TCPConnection
{
public:
	TCPConnection()
		:	isConnected( false )
	{

	}
	bool Init( const std::string &host, unsigned short port, bool server );
	bool ResolveHost();
	bool OpenConnectionToHost( );

	bool CheckForActivity() const;
	void Send( std::string str );
	std::string ReadMessages();

	bool IsConnected() const;

	void Close();

	void Update();
	bool StartServer( );
	bool AcceptConnection();
	bool SetServerSocket();
private:
	void* ConvertStringToVoidPtr( const std::string &str );

	bool isServer;
	std::string hostName;
	unsigned short portNr;
	bool isConnected;
	IPaddress ipAddress;
	int bufferSize;

	TCPsocket tcpSocket;
	TCPsocket serverSocket;
	SDLNet_SocketSet socketSet;
};
