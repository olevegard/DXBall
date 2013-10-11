#pragma once

#include "TCPConnection.h"

class NetManager
{
	public:
		void Init( bool server, std::string ip = "127.0.0.1", unsigned short port = 2002 )
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
		void AcceptConnection()
		{
			gameServer.AcceptConnection();

		}
		void Sent( std::string str )
		{
			gameServer.Send( str );
		}
		void Close()
		{ 
			gameServer.Close();

		}
		void Update()
		{
			if ( isServer )
			{
				gameServer.ReadMessages();
				gameServer.Send( "Sent from server" );
			}
			else
			{
				gameClient.Send( "Sent from client" );
				gameClient.ReadMessages();
			}
		}
		
		std::string ReadMessage( )
		{
			if ( isServer )
				return gameServer.ReadMessages();
			else
				return gameClient.ReadMessages();
		}
		void SendMessage( std::string str )
		{
			if ( isServer )
				gameServer.Send( str );
			else
				gameClient.Send( str );
		}
		bool IsServer()
		{
			return isServer;
		}
		bool IsConnected()
		{
			if ( isServer )
				return gameServer.IsConnected();
			else
				return gameClient.IsConnected();

		}
	private:
		bool isServer;
		TCPConnection gameServer;
		TCPConnection gameClient;
};	
