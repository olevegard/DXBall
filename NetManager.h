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
				gameServer.Init( "127.0.0.1", 2002, true );

				gameServer.StartServer();
				gameServer.ReadMessages();
			} else 
			{
				// Temporary code to give the server some time to start up...
				SDL_Delay( 100 );
				gameClient.Init( "127.0.0.1", 2002, false );

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
