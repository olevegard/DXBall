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

		void SendMessageToServer( std::string str );
		std::string GetIPAdress()
		{
			return ipAdress;
		}
		uint16_t GetPort()
		{
			return portNr;
		}
	private:
		std::string ipAdress;
		uint16_t portNr;

		bool isServer;
		TCPConnection mainServer;
		TCPConnection gameServer;
		TCPConnection gameClient;
};
