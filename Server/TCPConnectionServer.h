#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <SDL2/SDL_net.h>

class TCPConnectionServer
{
public:
	bool Init( const std::string &host, unsigned short port, bool server );
	bool ResolveHost();
	bool OpenConnectionToHost( );

	bool CheckForActivity( int connectionNr ) const;
	void Send( std::string str, int connectionNr );
	std::string ReadMessages( int connectionNr );

	bool IsConnected() const;

	bool StartServer( );
	bool AcceptConnection();
	bool SetServerSocket();
	void GetServerInfo( std::string &str, uint32_t port );
	bool Update();

	void Close();

private:
	void* ConvertStringToVoidPtr( const std::string &str );

	bool isServer;
	std::string hostName;
	unsigned short portNr;

	int bufferSize;

	IPaddress ipAddress;
	TCPsocket tcpSocket;
	SDLNet_SocketSet socketSet;

	std::vector< TCPsocket > serverSocket;
	std::vector< bool > isSocketConnected;
};
