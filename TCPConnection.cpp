#include "TCPConnection.h"
#include <csignal>

#include <iostream>

bool TCPConnection::Init( const std::string &host, unsigned short port, bool server )
{
	bufferSize = 1024;
	isConnected = false;

	isServer = server;
	hostName = host;
	portNr = port;
	socketSet = SDLNet_AllocSocketSet( 1 );

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

	isConnected = !server;
	return true;
}

bool TCPConnection::ResolveHost()
{
	int success = 0;

	if ( isServer )
		// IP is null becasue this is a server
		// Port is listening port
		success = SDLNet_ResolveHost( &ipAddress, nullptr, portNr );
	else
		// IP is the IP of the server
		// Port is the port the serer listens on ( seea above )
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

bool TCPConnection::OpenConnectionToHost( )
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

void TCPConnection::Send( std::string str )
{
	if ( !isConnected  )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Error! Not connected " << std::endl;
		return;
	}

	void* messageData = ConvertStringToVoidPtr(str);
	int messageSize = static_cast< int > ( str.length() );
	int bytesSent = 0;

	if ( isServer )
	{
		bytesSent = SDLNet_TCP_Send( serverSocket,  messageData,  messageSize);
	}
	else
	{
		bytesSent = SDLNet_TCP_Send( tcpSocket,  messageData,  messageSize);
	}

	if ( bytesSent < messageSize )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Send failed : " << SDLNet_GetError() << std::endl;
		isConnected = false;
	}
}
void TCPConnection::Close()
{
	if ( !isConnected )
		return;

	std::cout << "TCPConnection.cpp@" << __LINE__ << " Closing connection" << std::endl;
	SDLNet_TCP_Close( tcpSocket );

	if ( SDLNet_TCP_DelSocket( socketSet, tcpSocket ) == -1 )
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Failed to delete socket : " << SDLNet_GetError()  << std::endl;

	SDLNet_FreeSocketSet( socketSet );

	if ( isServer )
		SDLNet_TCP_Close( serverSocket );

	isConnected = false;
}


void TCPConnection::Update()
{
	if ( !isServer || isConnected )
		return;

	if ( AcceptConnection() )
	{
		if ( SetServerSocket() )
		{
			SDLNet_TCP_AddSocket( socketSet, serverSocket );
			isConnected = true;
		}
	}
}
bool TCPConnection::StartServer( )
{
	if (!isServer )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Not in server mode, can't start server" << std::endl;
		return false;
	}

	Update();

	return isConnected;
}

bool TCPConnection::AcceptConnection()
{
	if ( tcpSocket == nullptr )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " tcpSoket is null" << std::endl;
		return false;
	}
	serverSocket = SDLNet_TCP_Accept( tcpSocket );

	if ( serverSocket  == nullptr )
	{
		//std::cout << "TCPConnection.cpp@" << __LINE__ << " Cannot accept TCP connection : " << SDLNet_GetError()  << std::endl;
		isConnected = false;
		return false;
	}

	return true;
}
bool TCPConnection::SetServerSocket()
{
	IPaddress* ipRemote = SDLNet_TCP_GetPeerAddress( serverSocket );

	if ( ipRemote == nullptr )
	{
		std::cout  << "TCPConnection.cpp@" << __LINE__
			<< " Failed to get peer addres : " << hostName << " : " << portNr
			<< "\n\tServer : " << std::boolalpha << isServer << std::endl;
		isConnected = false;
		return false;
	}

	std::cout << "TCPConnection.cpp@" << __LINE__
		<< " Host connected : "
		<< SDLNet_Read32( &ipRemote->host )
		<< " : " << SDLNet_Read16( &ipRemote->port ) << std::endl;

	isConnected = true;
	return true;
}
std::string TCPConnection::ReadMessages()
{
	if ( !CheckForActivity() )
		return "";

	char buffer[bufferSize];
	memset( buffer, 0, bufferSize );

	int byteCount  = 0;
	std::string received("");

	if ( !isConnected )
		return received;

	if ( isServer )
		byteCount = SDLNet_TCP_Recv( serverSocket, buffer, bufferSize );
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
		isConnected = false;
		// A bytecount of < 0 means an error occured
	} else if ( byteCount < 0 )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Read failed!" <<
			"\nSocket : " << ( isServer ? serverSocket : tcpSocket )  <<
			"\nByte count : " << byteCount <<
			"\nERrror : " << SDLNet_GetError() <<
			std::endl;
	}

	return received;
}
bool TCPConnection::CheckForActivity() const
{
	int countReady = SDLNet_CheckSockets( socketSet, 0 );

	if ( countReady < 0 )
	{
		std::cout << "TCPConnection.cpp@" << __LINE__ << " Error! " << SDLNet_GetError() << std::endl;
		return false;
	}

	if ( isServer )
		return SDLNet_SocketReady( serverSocket ) != 0;
	else
		return SDLNet_SocketReady( tcpSocket ) != 0;
}
bool TCPConnection::IsConnected() const
{
	return isConnected;
}

void* TCPConnection::ConvertStringToVoidPtr( const std::string &str )
{
	const char* charPtr = str.c_str();

	return const_cast< char* > ( charPtr );
}
