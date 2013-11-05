#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#include "enums/MessageType.h"
#include "enums/BonusType.h"

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

		BonusType GetBonusType() const;

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

		void SetBonusType( BonusType bonustType_ );

		void SetXPos( double xPos_ );
		void SetYPos( double yPos_ );
		void SetXDir( double xDir_ );
		void SetYDir( double yDir_ );
		void SetXSize( double xSize_ );
		void SetYSize( double ySize_ );
		void SetBoardScale( double boardScale_);
	private:
		MessageType msgType;
		unsigned int objectID;

		BonusType bonusType;

		double xPos;
		double yPos;

		double xDir;
		double yDir;

		double xSize;
		double ySize;

		double boardScale;
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
		case BallKilled:
		case TileHit:
		case BonusPickup:
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

				msg.SetBonusType( static_cast< BonusType > ( bonusType ) );

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
		case BallKilled:
		case TileHit:
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
			os << static_cast< int32_t > ( message.GetBonusType() );
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
		default:
			std::cout << "Wrong message type : " << type << std::endl;
			std::cin.ignore();
			break;
	}
	return os;
}
