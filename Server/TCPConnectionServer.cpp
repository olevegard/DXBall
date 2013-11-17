#include "TCPConnectionServer.h"

#include <SDL2/SDL_endian.h>

#include <iostream>
#include <sstream>

bool TCPConnectionServer::Init( const std::string &host, unsigned short port, bool server )
{
	bufferSize = 1024;

	isServer = server;
	hostName = host;
	portNr = port;

	socketSet = SDLNet_AllocSocketSet( 100 );
	isSocketConnected.resize( 100 );

	for ( int i = 0 ; i < isSocketConnected.size() ; ++i )
		isSocketConnected[i] = false;

	if ( !ResolveHost() )
	{
		std::cout << "TCPConnection@" << __LINE__ << " Resolve Host failed!" << std::endl;
		return false;
	}

	if ( !OpenConnectionToHost() )
	{
		std::cout << "TCPConnection@" << __LINE__ << " Open Connection To  Host failed!" << std::endl;
		return false;
	}

	return true;
}

bool TCPConnectionServer::ResolveHost()
{
	int success = 0;

	if ( isServer )
		success = SDLNet_ResolveHost( &ipAddress, nullptr, portNr );
	else
		success = SDLNet_ResolveHost( &ipAddress, hostName.c_str(), portNr );

	if ( success == -1 )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Failed to open port host :"
			"\nIP Adress : " << hostName <<
			"\nPort : " << portNr <<
			"\nError : " << SDLNet_GetError() <<
			"\nLine : " << __LINE__
			<< std::endl;
		return false;
	}

	return true;
}

bool TCPConnectionServer::OpenConnectionToHost( )
{
	tcpSocket = SDLNet_TCP_Open( &ipAddress );

	if ( !isServer )
		SDLNet_TCP_AddSocket( socketSet, tcpSocket );

	if ( tcpSocket == nullptr )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Failed to open port host :"
			"\nIP Adress : " << hostName <<
			"\nPort : " << portNr <<
			"\nError : " << SDLNet_GetError() <<
			"\nLine : " << __LINE__
			<< std::endl;
		return false;

	}
	return true;
}

void TCPConnectionServer::Send( std::string str, int connectionNr  )
{
	if ( !isSocketConnected[ connectionNr ]  )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Error! Not connected " << std::endl;
		return;
	}

	void* messageData = ConvertStringToVoidPtr(str);
	int messageSize = static_cast< int > ( str.length() );
	int bytesSent = 0;

	if ( isServer )
	{
		bytesSent = SDLNet_TCP_Send( serverSocket[connectionNr],  messageData,  messageSize);
	}
	else
	{
		bytesSent = SDLNet_TCP_Send( tcpSocket,  messageData,  messageSize);
	}

	if ( bytesSent < messageSize )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Send failed : " << SDLNet_GetError() << std::endl;
		isSocketConnected[connectionNr] = false;
	}
}
void TCPConnectionServer::Close()
{
	if ( isSocketConnected[ 0 ]  )
		SDLNet_TCP_Close( serverSocket[ 0 ]  );
}
bool TCPConnectionServer::StartServer( )
{
	if (!isServer )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Not in server mode, can't start server" << std::endl;
		return false;
	}

	bool quit = false;
	while ( !quit )
	{
		if ( AcceptConnection() )
		{

			std::cout << "TCPConnection.cpp@" << __LINE__ << "Accepted connection...\n";
			if ( SetServerSocket() )
			{
				SDLNet_TCP_AddSocket( socketSet, serverSocket[ serverSocket.size() - 1] );

				quit = true;
				return true;
			}
		}
		SDL_Delay( 20 );
	}
	return false;
}
bool TCPConnectionServer::Update()
{
	if ( AcceptConnection() )
	{
		if ( SetServerSocket() )
		{
			std::cout << "Adding Socket : "<< serverSocket.size() - 1 << std::endl;
			SDLNet_TCP_AddSocket( socketSet, serverSocket[ serverSocket.size() - 1] );
			return true;
		}
	}

	return false;
}
bool TCPConnectionServer::AcceptConnection()
{
	TCPsocket socket = SDLNet_TCP_Accept( tcpSocket );

	if ( socket == nullptr )
	{
		// Connection failed, this just means no new client is trying to connect
		return false;
	}

	serverSocket.push_back(socket  );
	isSocketConnected[ serverSocket.size() - 1 ] = true;
	return true;
}
bool TCPConnectionServer::SetServerSocket()
{
	IPaddress* ipRemote = SDLNet_TCP_GetPeerAddress( serverSocket[ serverSocket.size() - 1] );

	if ( ipRemote == nullptr )
	{
		std::cout  << "TCPConnection.cpp@" << __LINE__
			<< " Failed to get peer addres : " << hostName << " : " << portNr
			<< "\n\tServer : " << std::boolalpha << isServer << std::endl;
		return false;
	}

	std::cout << "TCPConnection.cpp@" << __LINE__
		<< " Host connected : "
		<< SDLNet_Read32( &ipRemote->host )
		<< " : " << SDLNet_Read16( &ipRemote->port ) << std::endl;

	return true;
}

void TCPConnectionServer::GetServerInfo( std::string &str, uint32_t prt)
{
	IPaddress* ipRemote = SDLNet_TCP_GetPeerAddress( serverSocket[ serverSocket.size() - 1] );
	uint8_t* ptr = reinterpret_cast<  uint8_t* > ( &ipRemote->host );

	std::stringstream ss;
	ss << "IP : "
		<< static_cast< int32_t > ( ptr[0] ) << "."
		<< static_cast< int32_t > ( ptr[1] ) << "."
		<< static_cast< int32_t > ( ptr[2] ) << "."
		<< static_cast< int32_t > ( ptr[3] );

	str = ss.str();

	prt = SDLNet_Read16( &ipRemote->port );
}
std::string TCPConnectionServer::ReadMessages( int connectionNr )
{
	if ( !CheckForActivity( connectionNr ) )
		return "";

	char buffer[bufferSize];
	memset( buffer, 0, bufferSize );

	int byteCount  = 0;
	std::string received("");

	if ( isSocketConnected.size() <  ( connectionNr + 1 ) )
		return received;

	if ( isServer )
		byteCount = SDLNet_TCP_Recv( serverSocket[connectionNr], buffer, bufferSize );
	else
		byteCount = SDLNet_TCP_Recv( tcpSocket, buffer, bufferSize );

	if ( byteCount > 0 )
	{
		//memset( &buffer[byteCount], 0, static_cast< size_t > (512 - byteCount ) );
		buffer[byteCount] = '\0';
		received = buffer;

		if ( byteCount >= bufferSize )
		{
			std::cout << "TCPConnection.cpp@" << __LINE__
				<< " Too much data received : " << byteCount
				<< " / " << bufferSize
				<< std::endl;
		}

	}
	// A bytecount of 0 means the connection has been terminated
	else if ( byteCount == 0 )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Connection terminated" << std::endl;
		isSocketConnected[ connectionNr ] = false;

	// A bytecount of < 0 means an error occured
	} else if ( byteCount < 0 )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Read failed!" <<
			"\nSocket : " << ( isServer ? serverSocket[connectionNr] : tcpSocket )  <<
			"\nByte count : " << byteCount <<
			"\nError : " << SDLNet_GetError() <<
			std::endl;
	}

	return received;
}
bool TCPConnectionServer::CheckForActivity( int connectionNr ) const
{
	if ( !isSocketConnected[ connectionNr ] )
		return false;

	int countReady = SDLNet_CheckSockets( socketSet, 0 );

	if ( countReady < 0 )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Error! " << SDLNet_GetError() << std::endl;
		return false;
	}

	if ( isServer )
		return SDLNet_SocketReady( serverSocket[connectionNr] ) != 0;
	else
		return SDLNet_SocketReady( tcpSocket ) != 0;
}
bool TCPConnectionServer::IsConnected() const
{
	return false;
}

void* TCPConnectionServer::ConvertStringToVoidPtr( const std::string &str )
{
	const char* charPtr = str.c_str();

	return const_cast< char* > ( charPtr );
}
