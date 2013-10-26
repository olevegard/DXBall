#include "NetManager.h"

#include <algorithm>

#include "TCPMessage.h"

void NetManager::Init( bool server, std::string ip, unsigned short port)
{
	isServer = server;
	if ( SDLNet_Init() < 0 )
	{
		std::cout << "Failed to init SDLNet : " << SDLNet_GetError() << std::endl;
		return;
	}

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


