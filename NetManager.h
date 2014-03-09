#pragma once

#include "structs/net/TCPConnection.h"

class NetManager
{
	public:
		void Init( bool server );
		void Connect( std::string IP, unsigned short port );
		void Close();
		void Update();

		std::string ReadMessage( );
		std::string ReadMessageFromServer( );

		void SendMessage( std::string str );
		void SendMessageToServer( std::string str );

		bool IsServer() const;
		bool IsConnected() const;

		std::string GetIPAdress();
		uint16_t GetPort();

		void SetIsServer( bool isServer_ );
	private:
		std::string ipAdress;
		uint16_t portNr;

		bool isServer;
		bool isReady;
		TCPConnection mainServer;
		TCPConnection gameServer;
		TCPConnection gameClient;
};
