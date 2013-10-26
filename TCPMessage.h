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

		// Setters
		void SetMessageType( MessageType msgType_ );
		void SetMessageType( int  msgType_ );
		void SetObjectID( unsigned int objectID_ );
		void SetXPos( double xPos_ );
		void SetYPos( double yPos_ );
		void SetXDir( double xDir_ );
		void SetYDir( double yDir_ );

	private:
		MessageType msgType;
		unsigned int objectID;
		double xPos;
		double yPos;
		double xDir;
		double yDir;
};

inline std::istream& operator>>( std::istream &is, TCPMessage &msg )
{
	int type = 0;
	unsigned int objectID = 0;

	is >> type;
	is >> objectID;

	msg.SetMessageType( type );
	msg.SetObjectID( objectID );

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
	double xDir = 0.0;
	double yDir = 0.0;

	is >> yPos;
	is >> xDir;
	is >> yDir;

	msg.SetYPos( yPos );
	msg.SetXDir( xDir );
	msg.SetYDir( yDir );

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const TCPMessage &pos )
{
	MessageType type = pos.GetType();

	os << pos.GetTypeAsInt();
	os << " ";
	os << pos.GetObjectID();
	os << " ";

	// BallKilled and Tile Hit only needs message type and ID
	if ( type == MessageType::BallKilled || type == MessageType::TileHit )
		return os;

	os << pos.GetXPos();
	os << " ";

	// Paddle position only has xPos
	if ( type == MessageType::PaddlePosition )
		return os;

	os << pos.GetYPos();
	os << " ";
	os << pos.GetXDir();
	os << " ";
	os << pos.GetYDir();
	os << " ";
	return os;
}
