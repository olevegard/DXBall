#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#include "enums/MessageType.h"

class TCPMessage
{
	public:
		TCPMessage()
			:	msgType( MessageType::PaddlePosition )
			,	objectID( 0 )
			,	xPos(0.0)
			,	yPos(0.0)
			,	xDir(0.0)
			,	yDir(0.0)
	{}
	double GetXPos()  const
	{
		return xPos;
	}
	std::string Print() const
	{
		std::stringstream ss;
		ss << "Type : " << std::left << std::setw( 10 ) << GetTypeAsString() << "| Object ID :  " << objectID;
		if ( msgType == MessageType::BallKilled || msgType == MessageType::TileHit )
		{
			ss << "\n";
		} else if ( msgType == MessageType::PaddlePosition )
		{
			ss << " : "  << xPos << "\n";
		} else
		{
			ss << " : "  << xPos << " , " << yPos << " , " << xDir << " , " << yDir << "\n";
		}

		return ss.str();
	}
	std::string GetTypeAsString() const
	{
		switch ( msgType )
		{
			case MessageType::PaddlePosition:
				return "Paddle Position";
			case MessageType::BallSpawned:
				return "Ball Spawned";
			case MessageType::BallData:
				return "Ball Data";
			case MessageType::BallKilled:
				return "Ball Killed";
			case MessageType::TileHit:
				return "Tile Hit";
			default:
				break;
		}

		return "Unknown";
	}
	MessageType msgType;
	unsigned int objectID;
	double xPos;
	double yPos;
	double xDir;
	double yDir;
};

inline std::istream& operator>>( std::istream &is, TCPMessage &msg )
{
	int type;
	is >> type;

	msg.msgType = static_cast< MessageType > ( type );
	is >> msg.objectID;

	if ( type == MessageType::BallKilled || type == MessageType::TileHit )
		return is;

	is >> msg.xPos;

	if ( type == MessageType::PaddlePosition )
		return is;

	is >> msg.yPos;
	is >> msg.xDir;
	is >> msg.yDir;

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const TCPMessage &pos )
{
	os << pos.msgType;
	os << " ";
	os << pos.objectID;
	os << " ";

	// Ball killed only needs message type and ID
	if ( pos.msgType == MessageType::BallKilled || pos.msgType == MessageType::TileHit )
		return os;

	os << pos.xPos;
	os << " ";

	// Paddle position always has the same Y-pos
	if ( pos.msgType == MessageType::PaddlePosition )
		return os;

	os << pos.yPos;
	os << " ";
	os << pos.xDir;
	os << " ";
	os << pos.yDir;
	os << " ";
	return os;
}
