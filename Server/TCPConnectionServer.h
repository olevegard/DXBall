#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL_net.h>

class TCPConnection
{
public:
	bool Init( const std::string &host, unsigned short port, bool server );
	bool ResolveHost();
	bool OpenConnectionToHost( );

	bool CheckForActivity( int connectionNr ) const;
	void Send( std::string str, int connectionNr );
	std::string ReadMessages( int connectionNr );

	bool IsConnected() const;

	void Close();

	bool StartServer( );
	bool AcceptConnection();
	bool SetServerSocket();
	void GetServerInfo( std::string &str, uint32_t port );
private:
	void* ConvertStringToVoidPtr( const std::string &str );

	bool isServer;
	std::string hostName;
	unsigned short portNr;
	bool isConnected;
	IPaddress ipAddress;
	int bufferSize;

	TCPsocket tcpSocket;
	std::vector< TCPsocket > serverSocket;
	SDLNet_SocketSet socketSet;
};
