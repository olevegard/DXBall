#pragma once

#include "TCPConnection.h"
#include <iostream>

class NetManager
{
	public:
		void Init( bool server );
		void Connect( std::string IP, unsigned short port );
		void Close();

		std::string ReadMessage( );
		void SendMessage( std::string str );

		bool IsServer() const;
		bool IsConnected() const;
	private:
		bool isServer;
		TCPConnection mainServer;
		TCPConnection gameServer;
		TCPConnection gameClient;
};
