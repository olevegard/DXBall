#include "TCPConnection.h"

#include "../../Logger.h"

#include <csignal>
#include <iostream>

TCPConnection::TCPConnection()
	:	isConnected( false )
	,   bufferSize( 80000 )
{
	logger = Logger::Instance();
}
bool TCPConnection::Init( const std::string &host, unsigned short port, bool server )
{
	isConnected = false;

	isServer = server;
	hostName = host;
	portNr = port;
	socketSet = SDLNet_AllocSocketSet( 1 );

	if ( !ResolveHost() )
		return false;

	if ( !OpenConnectionToHost() )
	{
		logger->Log( __FILE__, __LINE__, "Opening connection to host faield : ", SDLNet_GetError() );
		return false;
	}

	isConnected = !server;
	return true;
}

bool TCPConnection::ResolveHost()
{
	int success = 0;
	std::string serverOrClient;

	if ( isServer )
	{
		// IP is null becasue this is a server
		// Port is listening port
		success = SDLNet_ResolveHost( &ipAddress, nullptr, portNr );
		serverOrClient = "server";
	}
	else
	{
		// IP is the IP of the server
		// Port is the port the serer listens on ( seea above )
		success = SDLNet_ResolveHost( &ipAddress, hostName.c_str(), portNr );
		serverOrClient = "client";
	}

	std::stringstream ss("");
	ss	<<  serverOrClient << "-" << hostName << " : " << portNr;
	logger->Log( __FILE__, __LINE__, "Resolving host : ", ss.str() );

	if ( success == -1 )
	{
		ss.str("");
		ss	<<  hostName << " : " << portNr <<" Error : " << SDLNet_GetError();
		logger->Log( __FILE__, __LINE__, "Failed to resolve host : ", ss.str() );

		return false;
	}

	logger->Log( __FILE__, __LINE__, "Host resolved!"  );

	return true;
}

bool TCPConnection::OpenConnectionToHost( )
{
	tcpSocket = SDLNet_TCP_Open( &ipAddress );

	if ( !isServer )
		SDLNet_TCP_AddSocket( socketSet, tcpSocket );

	if ( tcpSocket == nullptr )
	{
		std::stringstream ss("");
		ss	<<  hostName << " : " << portNr <<" Error : " << SDLNet_GetError();
		logger->Log( __FILE__, __LINE__, "Failed to port to host : ", ss.str() );
	}

	std::stringstream ss("");
	ss	<<  hostName << " : " << portNr;
	logger->Log( __FILE__, __LINE__, "Connection opened : ", ss.str() );

	return true;
}

void TCPConnection::Send( std::string str )
{
	if ( !isConnected  )
		return;

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
		logger->Log( __FILE__, __LINE__, "Send failed : ", SDLNet_GetError() );
		Close();
	}
}
void TCPConnection::Close()
{
	logger->Log( __FILE__, __LINE__, "Closing connection.." );
	if ( !isConnected )
	{
		logger->Log( __FILE__, __LINE__, "Can't close : not connected" );
		return;
	}

	if ( isServer )
	{
		SDLNet_TCP_DelSocket( socketSet, serverSocket );
		SDLNet_TCP_Close( serverSocket );
		SDLNet_FreeSocketSet( socketSet );
	}
	else
	{
		SDLNet_TCP_Close( tcpSocket );
	}

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
		logger->Log( __FILE__, __LINE__, "Can't start server : not in server mode" );
		return false;
	}

	Update();

	return isConnected;
}

bool TCPConnection::AcceptConnection()
{
	if ( tcpSocket == nullptr )
	{
		logger->Log( __FILE__, __LINE__, "TCP Socket is null" );
		return false;
	}
	serverSocket = SDLNet_TCP_Accept( tcpSocket );

	if ( serverSocket  == nullptr )
	{
		logger->Log( __FILE__, __LINE__, " Cannot accept TCP connection : ", SDLNet_GetError() );
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
		std::stringstream  ss( "" );
			ss << hostName << " : " << portNr
			<< "\n\tServer : " << std::boolalpha << isServer;
		isConnected = false;

		logger->Log( __FILE__, __LINE__, " Failed to get peer addres : ", ss.str() );
		return false;
	}

	std::stringstream ss("");
	ss << ipRemote->host << " : " << ipRemote->port << std::endl;
	logger->Log( __FILE__, __LINE__, " Host connected : ", ss.str() );

	isConnected = true;
	return true;
}
std::string TCPConnection::ReadMessages()
{
	if ( !CheckForActivity() )
		return "";

	char buffer[bufferSize ];
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
			logger->Log( __FILE__, __LINE__, " Too much data reciebed : ", byteCount );
			logger->Log( __FILE__, __LINE__, " ...Buffer size is : ", bufferSize );
		}
	}
	// A bytecount of 0 means the connection has been terminated
	else if ( byteCount == 0 )
	{
		logger->Log( __FILE__, __LINE__, "Connection terminated : " );
		Close();
	// A bytecount of < 0 means an error occured
	}
	else if ( byteCount < 0 )
	{
		logger->Log( __FILE__, __LINE__, "Read Failed : ", SDLNet_GetError() );
	}

	return received;
}
bool TCPConnection::CheckForActivity() const
{
	int countReady = SDLNet_CheckSockets( socketSet, 0 );

	if ( countReady < 0 )
	{
		logger->Log( __FILE__, __LINE__, "Error : ", SDLNet_GetError() );
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
