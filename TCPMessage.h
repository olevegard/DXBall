#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#include "enums/MessageType.h"
#include "enums/BonusType.h"
#include "enums/GameState.h"

class TCPMessage
{
	public:
		TCPMessage();
		std::string Print() const;

		// Getters
		std::string GetTypeAsString() const;
		int GetTypeAsInt() const;
		MessageType GetType() const;
		unsigned int GetObjectID() const;

		std::string GetBonusTypeAsString() const;
		BonusType GetBonusType() const;
		int32_t GetBonusTypeAsInt() const;

		std::string GetGameStateAsString() const;
		GameState GetGameState() const;
		int32_t GetGameStateAsInt() const;

		double GetXPos() const;
		double GetYPos() const;
		double GetXDir() const;
		double GetYDir() const;
		double GetXSize() const;
		double GetYSize() const;
		double GetBoardScale() const;

		// Setters
		void SetMessageType( MessageType msgType_ );
		void SetMessageType( int  msgType_ );
		void SetObjectID( unsigned int objectID_ );

		void SetBonusType( int32_t bonustType_ );
		void SetBonusType( BonusType bonustType_ );

		void SetGameState( int32_t bonustType_ );
		void SetGameState( GameState bonustType_ );

		void SetXPos( double xPos_ );
		void SetYPos( double yPos_ );
		void SetXDir( double xDir_ );
		void SetYDir( double yDir_ );
		void SetXSize( double xSize_ );
		void SetYSize( double ySize_ );
		void SetBoardScale( double boardScale_);

		void SetPort( uint16_t port_ )
		{
			port = port_;
		}
		void SetIPAdress( std::string  ipAddress_ )
		{
			ipAddress = ipAddress_;
		}

		void SetPlayerName( std::string playerName_ )
		{
			playerName = playerName_;
		}

		uint16_t GetPort() const
		{
			return port;
		}
		std::string GetIPAdress() const
		{
			return ipAddress;
		}
		std::string GetPlayerName() const
		{
			return playerName;
		}
	private:
		MessageType msgType;
		unsigned int objectID;

		BonusType bonusType;

		GameState newGameState;

		double xPos;
		double yPos;

		double xDir;
		double yDir;

		double xSize;
		double ySize;

		double boardScale;

		std::string ipAddress;
		uint16_t port;

		std::string playerName;
};

inline std::istream& operator>>( std::istream &is, TCPMessage &msg )
{
	int type = 0;
	unsigned int objectID = 0;

	is >> type;
	is >> objectID;

	msg.SetMessageType( type );
	msg.SetObjectID( objectID );

	switch ( type )
	{
		// BallKilled and Tile Hit only needs message type and ID
		case TileHit:
		case GameJoined:
		case BallKilled:
		case BonusPickup:
		case GetGameList:
			return is;
		// Game Settings uses xSize and ySize
		case GameSettings:
			{
				double xSize = 0.0;
				double ySize = 0.0;
				double boardScale = 0.0;

				is >> xSize;
				is >> ySize;

				msg.SetXSize( xSize );
				msg.SetYSize( ySize );

				is >> boardScale;
				msg.SetBoardScale( boardScale );

				return is;
			}
		// Paddle position only has xPos
		case PaddlePosition:
			{
				double xPos = 0.0;


				is >> xPos;

				msg.SetXPos( xPos );
				return is;
			}
		// BallData has both pos and dir
		case BallSpawned:
		case BallData:
		case BonusSpawned:
			{
				int bonusType;
				is >> bonusType;

				msg.SetBonusType( bonusType );

				double xPos = 0.0;
				double yPos = 0.0;

				is >> xPos;
				is >> yPos;

				msg.SetXPos( xPos );
				msg.SetYPos( yPos );

				double xDir = 0.0;
				double yDir = 0.0;

				is >> xDir;
				is >> yDir;

				msg.SetXDir( xDir );
				msg.SetYDir( yDir );

				return is;
			}
		case GameStateChanged:
			{
				int gameState = 0;
				is >> gameState;
				msg.SetGameState( gameState );
				return is;
			}
		case EndGame:
		case NewGame:
			{
				std::string ip;
				is >> ip;
				msg.SetIPAdress( ip );

				uint16_t port;
				is >> port;
				msg.SetPort( port );

				return is;
			}
		case PlayerName:
			{
				std::string playerName_;
				is >> playerName_;
				msg.SetPlayerName( playerName_ );

				return is;
			}
		default:
			{
				std::cout << "Wrong message type : " << type << std::endl;
				std::cin.ignore();
				return is;
			}
	}

}
inline std::ostream& operator<<( std::ostream &os, const TCPMessage &message )
{
	MessageType type = message.GetType();

	os << message.GetTypeAsInt();
	os << " ";
	os << message.GetObjectID();
	os << " ";

	switch ( type )
	{
		// BallKilled and Tile Hit only needs message type and ID
		case TileHit:
		case GameJoined:
		case BallKilled:
		case GetGameList:
		case BonusPickup:
			break;
		// Game Settings uses xSize and ySize
		case GameSettings:
			os << message.GetXSize();
			os << " ";
			os << message.GetYSize();
			os << " ";

			os << message.GetBoardScale();
			os << " ";

			break;
		// Paddle position only has xPos
		case PaddlePosition:
			os << message.GetXPos();
			os << " ";

			break;
		// BallData has both pos and dir
		case BallSpawned:
		case BallData:
		case BonusSpawned:
			os << message.GetBonusTypeAsInt();
			os << " ";
			os << message.GetXPos();
			os << " ";
			os << message.GetYPos();
			os << " ";

			os << message.GetXDir();
			os << " ";
			os << message.GetYDir();
			os << " ";

			break;
		case GameStateChanged:
			os << message.GetGameStateAsInt() << " ";
			break;
		case EndGame:
		case NewGame:
			os << message.GetIPAdress() << " ";
			os << message.GetPort() << " ";
			break;
		case PlayerName:
			os << message.GetPlayerName();
			break;
		default:
			std::cout << "Wrong message type : " << type << std::endl;
			std::cin.ignore();
			break;
	}
	return os;
}
