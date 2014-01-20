#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdint>

#include "../../enums/MessageType.h"
#include "../../enums/BonusType.h"
#include "../../enums/GameState.h"

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
		unsigned int GetObjectID2() const;

		std::string GetBonusTypeAsString() const;
		BonusType GetBonusType() const;
		int32_t GetBonusTypeAsInt() const;

		std::string GetGameStateAsString() const;
		GameState GetGameState() const;
		int32_t GetGameStateAsInt() const;

		double GetXPos() const;
		double GetYPos() const;
		double GetXPos2() const;
		double GetYPos2() const;
		double GetXDir() const;
		double GetYDir() const;
		double GetXSize() const;
		double GetYSize() const;
		double GetBoardScale() const;

		// Setters
		void SetMessageType( MessageType msgType_ );
		void SetMessageType( int  msgType_ );
		void SetObjectID( unsigned int objectID_ );
		void SetObjectID2( unsigned int objectID_ );

		void SetBonusType( int32_t bonustType_ );
		void SetBonusType( BonusType bonustType_ );

		void SetGameState( int32_t bonustType_ );
		void SetGameState( GameState bonustType_ );

		void SetXPos( double xPos_ );
		void SetYPos( double yPos_ );
		void SetXPos2( double xPos_ );
		void SetYPos2( double yPos_ );
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
		unsigned int objectID2;

		BonusType bonusType;

		GameState newGameState;

		double xPos;
		double yPos;

		double xPos2;
		double yPos2;

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

	is >> type >> objectID;

	msg.SetMessageType( type );
	msg.SetObjectID( objectID );

	switch ( type )
	{
		// BallKilled and Tile Hit only needs message type and ID
		case TileHit:
		case LevelDone:
		case GameJoined:
		case BallKilled:
		case BonusPickup:
		case GetGameList:
		case BulletKilled:
			return is;
		// Game Settings uses xSize and ySize
		case GameSettings:
			{
				double xSize = 0.0;
				double ySize = 0.0;
				double boardScale = 0.0;

				is >> xSize  >> ySize >> boardScale;

				msg.SetXSize( xSize );
				msg.SetYSize( ySize );
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
				double xPos = 0.0;
				double yPos = 0.0;
				double xDir = 0.0;
				double yDir = 0.0;

				is >> bonusType >> xPos  >> yPos  >> xDir >> yDir;

				msg.SetBonusType( bonusType );
				msg.SetXPos( xPos );
				msg.SetYPos( yPos );
				msg.SetXDir( xDir );
				msg.SetYDir( yDir );

				return is;
			}
		case BulletFire:
			{
				int32_t ID2 = 0;
				double xPos = 0.0;
				double yPos = 0.0;
				double xPos2 = 0.0;
				double yPos2 = 0.0;

				is >> ID2 >> xPos >> yPos  >> xPos2  >> yPos2;

				msg.SetObjectID2( ID2 );
				msg.SetXPos( xPos );
				msg.SetYPos( yPos );
				msg.SetXPos2( xPos2 );
				msg.SetYPos2( yPos2 );

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

				uint16_t port;

				is >> ip  >> port;

				msg.SetIPAdress( ip );
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

	os
		<< message.GetTypeAsInt()  << " "
		<< message.GetObjectID()  << " ";

	switch ( type )
	{
		// BallKilled and Tile Hit only needs message type and ID
		case TileHit:
		case LevelDone:
		case GameJoined:
		case BallKilled:
		case GetGameList:
		case BonusPickup:
		case BulletKilled:
			break;
		// Game Settings uses xSize and ySize
		case GameSettings:
			os
				<< message.GetXSize() << " "
				<< message.GetYSize()  << " "
				<< message.GetBoardScale() << " ";
			break;
		// Paddle position only has xPos
		case PaddlePosition:
			os
				<< message.GetXPos() << " ";
			break;
		// BallData has both pos and dir
		case BallSpawned:
		case BallData:
		case BonusSpawned:
			os
				<< message.GetBonusTypeAsInt()  << " "
				<< message.GetXPos() << " "
				<< message.GetYPos() << " "
				<< message.GetXDir() << " "
				<< message.GetYDir() << " ";
			break;
		case BulletFire:
			os
				<< message.GetObjectID2() << " "
				<< message.GetXPos() << " "
				<< message.GetYPos() << " "
				<< message.GetXPos2() << " "
				<< message.GetYPos2() << " ";
			break;
		case GameStateChanged:
			os
				<< message.GetGameStateAsInt() << " ";
			break;
		case EndGame:
		case NewGame:
			os
				<< message.GetIPAdress() << " "
				<< message.GetPort() << " ";
			break;
		case PlayerName:
			os
				<< message.GetPlayerName() << " ";
			break;
		default:
			std::cout << "Wrong message type : " << type << std::endl;
			std::cin.ignore();
			break;
	}
	return os;
}
