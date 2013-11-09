#include "NetManager.h"

#include <algorithm>

#include "TCPMessage.h"

void NetManager::Init( bool server)
{
	isServer = server;

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

	if ( isServer )
	{
		gameServer.Init( ip, port, true );
		gameServer.StartServer();
	} else
	{
		// Temporary code to give the server some time to start up...
		SDL_Delay( 100 );
		gameClient.Init( ip, port, false );
	}
}
void NetManager::Close()
{
	gameServer.Close();
}
void NetManager::Update()
{
	gameServer.Update();
}
std::string NetManager::ReadMessage( )
{
	if ( isServer )
		return gameServer.ReadMessages();
	else
		return gameClient.ReadMessages();
}
void NetManager::SendMessage( std::string str )
{
	if ( isServer )
		gameServer.Send( str );
	else
		gameClient.Send( str );
}
void NetManager::SendMessageToServer( std::string str )
{
	mainServer.Send( str );
}
bool NetManager::IsServer() const
{
	return isServer;
}
bool NetManager::IsConnected() const
{
	if ( isServer )
		return gameServer.IsConnected();
	else
		return gameClient.IsConnected();
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
