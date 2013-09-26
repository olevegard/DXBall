#pragma once

#include "TCPConnection.h"

class NetManager
{
	public:
		void Init( bool isServer )
		{
			if ( SDLNet_Init() < 0 )
			{
				std::cout << "Failed to init SDLNet : " << SDLNet_GetError() << std::endl;
				return;
			}

			if ( isServer )
			{
				std::cout << "Initing game server..." << std::endl;
				gameServer.Init( "127.0.0.1", 2002, true );

				std::cout << "Accepting..." << std::endl;
				gameServer.StartServer();

				std::cout << "Reading message..." << std::endl;
				gameServer.ReadMessages();
			} else 
			{
				std::cout << "Initing game client..." << std::endl;
				gameClient.Init( "127.0.0.1", 2002, false );

				std::cout << "Sending message..." << std::endl;
				gameClient.Send( "Hello" );
			}

			//
			//gameServer.Close();
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
	private:
		TCPConnection gameServer;
		TCPConnection gameClient;
};	
