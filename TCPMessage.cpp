#include "TCPMessage.h"

TCPMessage::TCPMessage()
	:	msgType( MessageType::PaddlePosition )
	,	objectID( 0 )
	,	xPos(0.0)
	,	yPos(0.0)
	,	xDir(0.0)
	,	yDir(0.0)
{}
std::string TCPMessage::Print() const
{
	std::stringstream ss;
	ss << "Type : " << std::left << std::setw( 16 ) << GetTypeAsString() << "| Object ID : "  << std::left << std::setw( 4 ) << objectID;

	if ( msgType == MessageType::BallKilled || msgType == MessageType::TileHit )
	{
		ss << "\n";
	}
	else if ( msgType == MessageType::PaddlePosition )
	{
		ss << " : "  << xPos << "\n";
	}
	else if ( msgType == MessageType::GameSettings )
	{
		ss << " : "  << xPos << " , " << yPos << "\n";
	}
	else
	{
		ss << " : "  << xPos << " , " << yPos << " , " << xDir << " , " << yDir << "\n";
	}

	return ss.str();
}
// Getters
std::string TCPMessage::GetTypeAsString() const
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
		case MessageType::GameSettings:
			return "Game Settings";
		default:
			break;
	}

	return "Unknown";
}
int TCPMessage::GetTypeAsInt() const
{
	return msgType;
}
MessageType TCPMessage::GetType() const
{
	return msgType;
}
unsigned int TCPMessage::GetObjectID() const
{
	return objectID;
}
double TCPMessage::GetXPos() const
{
	return xPos;
}
double TCPMessage::GetYPos() const
{
	return yPos;
}
double TCPMessage::GetXDir() const
{
	return xDir;
}
double TCPMessage::GetYDir() const
{
	return yDir;
}
// Setters
void TCPMessage::SetMessageType( MessageType msgType_ )
{
	msgType = msgType_;
}
void TCPMessage::SetMessageType( int  msgType_ )
{
	msgType = static_cast< MessageType > ( msgType_ );
}
void TCPMessage::SetObjectID( unsigned int objectID_ )
{
	objectID = objectID_;
}
void TCPMessage::SetXPos( double xPos_ )
{
	xPos = xPos_;
}
void TCPMessage::SetYPos( double yPos_ )
{
	yPos = yPos_;
}
void TCPMessage::SetXDir( double xDir_ )
{
	xDir = xDir_;
}
void TCPMessage::SetYDir( double yDir_ )
{
	yDir = yDir_;
}
