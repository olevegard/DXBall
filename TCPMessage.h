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
			,	xPos(0.0)
			,	yPos(0.0)
			,	xDir(0.0)
			,	yDir(0.0)
	{}
		MessageType msgType;	
	double xPos;
	double yPos;
	double xDir;
	double yDir;
	double GetXPos()  const
	{
		return xPos;
	}
	std::string Print() const
	{
		std::stringstream ss;
		ss << "Type : " << GetTypeAsString() << " : " << xPos << " , " << yPos << " , " << xDir << " , " << yDir << "\n";
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
			default:
				break;
		}

		return "Unknown";
	}

};



inline std::istream& operator>>( std::istream &is, TCPMessage &msg )
{
	int type;
	is >> type;
	msg.msgType = static_cast< MessageType > ( type );

	if ( type == MessageType::BallKilled )
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
	if ( pos.msgType == MessageType::BallKilled )
		return os;
	os << pos.xPos;
	os << " ";
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
