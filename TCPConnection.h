#pragma once

#include <string>
#include <iostream>

#include <SDL2/SDL_net.h>

class TCPConnection
{
public:
	bool Init( const std::string &host, unsigned short port )
	{
		isConnected = false;


		hostName = host;
		portNr = port;
		std::cout << "Initialising" << std::endl;

		if ( !ResolveHost() )
			return false;

		if ( !OpenConnectionToHost() )
			return false;

		isConnected = true;
		return true;
	}
	bool ResolveHost()
	{
		if ( SDLNet_ResolveHost( &ipAddress, hostName.c_str(), portNr ) == -1 )
		{
			std::cout << "Failed to open port host :"
				"\nIP Adress : " << hostName <<
				"\nPort : " << portNr <<
				"\nError : " << SDLNet_GetError()
				<< std::endl;
			return false;
		}

		return true;
	}

	bool OpenConnectionToHost( )
	{
		tcpSocket = SDLNet_TCP_Open( &ipAddress );

		if ( tcpSocket == nullptr )
		{
			std::cout << "Failed to open port host :"
				"\nIP Adress : " << hostName <<
				"\nPort : " << portNr <<
				"\nError : " << SDLNet_GetError()
				<< std::endl;
			return false;

		}
		return true;
	}

		
	void Send( std::string str )
	{
		if ( !isConnected  )
		{
			std::cout << "Error! Not connected " << std::endl;
			return;
		}

		void* messageData = ConvertStringToVoidPtr(str);
		int messageSize = static_cast< int > ( str.length() );

		if ( SDLNet_TCP_Send( tcpSocket,  messageData,  messageSize) < messageSize ) 
		{
			std::cout << "Send failed : " << SDLNet_GetError() << std::endl;
		}

	}
	void Close()
	{
		if ( isConnected )
			SDLNet_TCP_Close( tcpSocket );

	}
	bool StartServer( )
	{
		bool quit = false;
		while ( !quit )
		{
			if ( AcceptConnection() )
			{
				if ( GetPeerAddress() )
				{
					quit = false;
					isConnected = true;
					return true;
				}
			}
		}
		return false;
	}

	bool AcceptConnection()
	{
		connectedSocket = SDLNet_TCP_Accept( tcpSocket );

		if ( connectedSocket  == nullptr ) 
		{
			std::cout << "Cannot accept TCP connection : " << SDLNet_GetError()  << std::endl;
			isConnected = false;	
			return false;
		}

		return true;
	}
	bool GetPeerAddress()
	{
		ipRemote = SDLNet_TCP_GetPeerAddress( tcpSocket );

		if ( ipRemote == nullptr ) 
		{
			std::cout << "Failed to get peer addres : " << hostName << " : " << portNr << std::endl;
			isConnected = false;	
			return false;
		}

		std::cout << "Host connected : " << SDLNet_Read32( &ipRemote->host ) << " : " << SDLNet_Read32( &ipRemote->host ) << std::endl;

		isConnected = true;	
		return true;
	}

private:
	void* ConvertStringToVoidPtr( const std::string &str )
	{
		const char* charPtr = str.c_str();

		return const_cast< char* > ( charPtr );
	}
	std::string hostName;
	unsigned short portNr;
	bool isConnected;
	IPaddress ipAddress;
	IPaddress* ipRemote;
	TCPsocket tcpSocket;
	TCPsocket connectedSocket;
};
