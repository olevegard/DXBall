#include "NetManager.h"

#include <algorithm>

#include "structs/net/TCPMessage.h"

void NetManager::Init( bool server)
{
	isServer = server;
	isReady = false;

	if ( SDLNet_Init() < 0 )
	{
		std::cout << "Failed to init SDLNet : " << SDLNet_GetError() << std::endl;
		return;
	}

	mainServer.Init( "127.0.0.1", 3113, false );
}
void NetManager::Connect( std::string ip, unsigned short port )
{
	ipAdress = ip;
	portNr = port;
	isReady = true;

	if ( isServer )
	{
		while ( !gameServer.Init( ip, port, true ) );
		gameServer.StartServer();
	} else
	{
		// Temporary code to give the server some time to start up...
		//SDL_Delay( 100 );
		while ( !gameClient.Init( ip, port, false ) );
	}
	std::cout << "Connected!!!!!!!\n";
}
void NetManager::Close()
{
	isReady = false;
	gameServer.Close();
}
void NetManager::Update()
{
	if ( !isReady )
		return;

	if ( !isServer || gameServer.IsConnected()  )
	{
		return;
	}
	gameServer.Update();
}
std::string NetManager::ReadMessage( )
{
	if ( isServer )
		return gameServer.ReadMessages();
	else
		return gameClient.ReadMessages();
}
std::string NetManager::ReadMessageFromServer( )
{
	return mainServer.ReadMessages();
}
void NetManager::SendMessage( std::string str )
{
	if ( isServer )
	{
		gameServer.Send( str );
	}
	else
	{
		gameClient.Send( str );
	}
}
void NetManager::SendMessageToServer( std::string str )
{
	mainServer.Send( str );
}
bool NetManager::IsServer() const
{
	return isServer;
}
// Setters and getters
// ===========================================================================
bool NetManager::IsConnected() const
{
	if ( isServer )
	{
		return gameServer.IsConnected();
	}
	else
	{
		return gameClient.IsConnected();
	}
}
std::string NetManager::GetIPAdress()
{
	return ipAdress;
}
uint16_t NetManager::GetPort()
{
	return portNr;
}
void NetManager::SetIsServer( bool isServer_ )
{
	isServer = isServer_;
}
bool NetManager::IsConnectedToGameServer() const
{
	return mainServer.IsConnected();
}
