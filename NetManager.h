#pragma once

#include "TCPConnection.h"
#include <iostream>

class NetManager
{
	public:
		void Init( bool server, std::string ip = "127.0.0.1", unsigned short port = 2002 );
		void Close();

		std::string ReadMessage( );
		void SendMessage( std::string str );

		bool IsServer() const;
		bool IsConnected() const;
	private:
		bool isServer;
		TCPConnection gameServer;
		TCPConnection gameClient;
};
