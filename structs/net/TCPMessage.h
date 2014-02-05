#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdint>

#include "../../enums/MessageType.h"
#include "../../enums/BonusType.h"
#include "../../enums/GameState.h"

#include "../../math/Vector2f.h"

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

		Vector2f GetDir() const
		{
			return dir;
		}
		Vector2f GetPos1() const
		{
			return pos1;
		}
		Vector2f GetPos2() const
		{
			return pos2;
		}
		Vector2f GetSize() const
		{
			return size;
		}
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
		void SetPos1( Vector2f pos )
		{
			pos1 = pos;
		}

		void SetPos2( Vector2f pos )
		{
			pos2 = pos;
		}

		void SetDir( Vector2f dir_ )
		{
			dir = dir_;
		}
		void SetSize( Vector2f size_ )
		{
			size = size_;
		}

	private:
		MessageType msgType;
		unsigned int objectID;
		unsigned int objectID2;

		BonusType bonusType;

		GameState newGameState;

		Vector2f dir;

		Vector2f size;

		Vector2f pos1;
		Vector2f pos2;

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
				Vector2f size_;
				double boardScale = 0.0;

				is >> size_ >> boardScale;

				msg.SetSize( size_ );
				msg.SetBoardScale( boardScale );

				return is;
			}
		// Paddle position only has xPos
		case PaddlePosition:
			{
				double pos = 0.0;

				is >> pos;

				msg.SetPos1( Vector2f( pos, 0.0 ) );
				return is;
			}
		// BallData has both pos and dir
		case BallSpawned:
			{
				Vector2f pos_;
				Vector2f dir_;

				is >> pos_ >> dir_;

				msg.SetPos1( pos_ );
				msg.SetDir( dir_ );
				return is;
			}
		case BallData:
		case BonusSpawned:
			{
				int bonusType;
				Vector2f pos;
				Vector2f dir;

				is >> bonusType >> pos >> dir;

				msg.SetBonusType( bonusType );
				msg.SetPos1( pos );
				msg.SetDir( dir );

				return is;
			}
		case BulletFire:
			{
				int32_t objectID2 = 0;
				Vector2f pos_;
				Vector2f pos2_;

				is >> pos_ >> objectID2  >> pos2_;

				msg.SetObjectID2( objectID2 );
				msg.SetPos1( pos_ );
				msg.SetPos2( pos2_ );

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
				<< message.GetSize() << " "
				<< message.GetBoardScale() << " ";
			break;
		// Paddle position only has xPos
		case PaddlePosition:
			os
				<< message.GetPos1().x << " ";
			break;
		// BallData has both pos and dir
		case BallSpawned:
			{
			os
				<< message.GetPos1() << " "
				<< message.GetDir() << " ";

			break;
			}
		case BallData:
		case BonusSpawned:
			os
				<< message.GetBonusTypeAsInt()  << " "
				<< message.GetPos1() << " "
				<< message.GetDir() << " ";
			break;
		case BulletFire:
			os
				<< message.GetPos1() << " "
				<< message.GetObjectID2() << " "
				<< message.GetPos2() << " ";
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
