#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdint>

#include "../../enums/MessageType.h"
#include "../../enums/BonusType.h"
#include "../../enums/GameState.h"
#include "../../enums/TileType.h"

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
		double GetBoardScale() const;

		TileType GetTileType() const;
		std::string GetTileTypeAsString() const;
		bool GetTileKilled() const;

		Vector2f GetDir() const;
		Vector2f GetDir_YFlipped() const;
		Vector2f GetPos1() const;
		Vector2f GetPos2() const;
		Vector2f GetSize() const;

		uint16_t GetPort() const;
		std::string GetIPAdress() const;
		std::string GetPlayerName() const;

		// Setters
		void SetMessageType( MessageType msgType_ );
		void SetMessageType( int  msgType_ );
		void SetObjectID( unsigned int objectID_ );
		void SetObjectID2( unsigned int objectID_ );

		void SetBonusType( int32_t bonustType_ );
		void SetBonusType( BonusType bonustType_ );

		void SetGameState( int32_t bonustType_ );
		void SetGameState( GameState bonustType_ );

		void SetTileType( TileType tileType_ );
		void SetTileKilled( bool isKilled );

		void SetBoardScale( double boardScale_);

		void SetPort( uint16_t port_ );
		void SetIPAdress( std::string  ipAddress_ );

		void SetPlayerName( std::string playerName_ );

		void SetPos1( Vector2f pos );
		void SetPos2( Vector2f pos );

		void SetDir( Vector2f dir_ );
		void SetSize( Vector2f size_ );

		void SetLevelName( std::string levelName_ )
		{
			levelName = levelName_;
		}

		std::string GetLevelName( ) const
		{
			return levelName;
		}
	private:
		std::string levelName;
		MessageType msgType;
		unsigned int objectID;
		unsigned int objectID2;

		BonusType bonusType;
		TileType tileType;
		GameState newGameState;

		Vector2f dir;

		Vector2f size;

		Vector2f pos1;
		Vector2f pos2;

		double boardScale;

		std::string ipAddress;
		uint16_t port;

		std::string playerName;

		bool tileKilled;
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
		case LevelDone:
		case GameJoined:
		case BallKilled:
		case BonusPickup:
		case GetGameList:
		case BulletKilled:
		case LastTileSent:
		case BallRespawn:
			return is;
		case TileHit:
			{
				bool killed = false;

				is >> killed;

				msg.SetTileKilled( killed );

				return is;
			}
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
		case TileSpawned:
			{
				Vector2f pos_;
				int32_t tileType_ = 0;

				is >> tileType_  >> pos_;

				msg.SetPos1( pos_ );
				msg.SetTileType( static_cast< TileType > ( tileType_ ) );
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
			{
				std::string ip;
				uint16_t port;

				is >> ip  >> port;

				msg.SetIPAdress( ip );
				msg.SetPort( port );

				return is;
			}
		case NewGame:
			{
				std::string ip;
				uint16_t port;
				std::string player;

				is >> ip  >> port >> player;

				msg.SetIPAdress( ip );
				msg.SetPort( port );
				msg.SetPlayerName( player );

				return is;
			}
		case PlayerName:
			{
				std::string playerName_;
				is >> playerName_;
				msg.SetPlayerName( playerName_ );

				return is;
			}
		case LevelName:
		{
			std::string levelName_;
			is >> levelName_;
			msg.SetLevelName( levelName_ );

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
		case LevelDone:
		case GameJoined:
		case BallKilled:
		case BallRespawn:
		case GetGameList:
		case BonusPickup:
		case BulletKilled:
		case LastTileSent:
			break;
		case TileHit:
			os << message.GetTileKilled() << " ";
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
		case TileSpawned:
			{
				os << static_cast< int32_t > ( message.GetTileType() ) << " " << message.GetPos1() << " ";

				break;
			}
		case LevelName:
		{
			os << message.GetLevelName() << " ";
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
			os
				<< message.GetIPAdress() << " "
				<< message.GetPort() << " ";
			break;
		case NewGame:
			os
				<< message.GetIPAdress() << " "
				<< message.GetPort() << " "
				<< message.GetPlayerName() << " ";
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
