#pragma once

#include "TCPConnection.h"

class NetManager
{
	public:
		void Init()
		{
			if ( SDLNet_Init() < 0 )
			{
				std::cout << "Failed to init SDLNet : " << SDLNet_GetError() << std::endl;
				return;
			}

			gameServer.Init( "127.0.0.1", 2002 );
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
};	
