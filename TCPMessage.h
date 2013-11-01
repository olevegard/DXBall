#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#include "enums/MessageType.h"

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


	// Game Settings uses xSize and ySize
	if ( type == MessageType::GameSettings )
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

	// BallKilled and Tile Hit only needs message type and ID
	if ( type == MessageType::BallKilled || type == MessageType::TileHit )
		return is;

	double xPos = 0.0;
	is >> xPos;
	msg.SetXPos( xPos );

	// Paddle position only has xPos
	if ( type == MessageType::PaddlePosition )
		return is;

	double yPos = 0.0;
	is >> yPos;
	msg.SetYPos( yPos );


	double xDir = 0.0;
	double yDir = 0.0;

	is >> xDir;
	is >> yDir;

	msg.SetYPos( yPos );
	msg.SetXDir( xDir );
	msg.SetYDir( yDir );

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const TCPMessage &message )
{
	MessageType type = message.GetType();

	os << message.GetTypeAsInt();
	os << " ";
	os << message.GetObjectID();
	os << " ";

	// Game Settings uses xSize and ySize
	if ( type == MessageType::GameSettings )
	{
		os << message.GetXSize();
		os << " ";
		os << message.GetYSize();
		os << " ";

		os << message.GetBoardScale();

		return os;
	}

	// BallKilled and Tile Hit only needs message type and ID
	if ( type == MessageType::BallKilled || type == MessageType::TileHit )
		return os;

	os << message.GetXPos();
	os << " ";

	// Paddle position only has xPos
	if ( type == MessageType::PaddlePosition )
		return os;

	os << message.GetYPos();
	os << " ";

	// Game Settings only has xPos and yPos
	if ( type == MessageType::GameSettings )
		return os;

	os << message.GetXDir();
	os << " ";
	os << message.GetYDir();
	os << " ";
	return os;
}
