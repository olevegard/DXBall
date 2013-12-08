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

	switch ( msgType )
	{
		case EndGame:
		case TileHit:
		case LevelDone:
		case GameJoined:
		case BallKilled:
		case GetGameList:
			ss << "\n";
			break;
		case PaddlePosition:
			ss << " : "  << xPos << "\n";
			break;
		case GameSettings:
			ss << " : "  << xSize << " , " << ySize << " board scale : " << boardScale <<  "\n";
			break;
		case BonusSpawned:
			ss << " : " << GetBonusTypeAsString() << " , "  << xPos << " , " << yPos << " , " << xDir << " , " << yDir << "\n";
			break;
		case GameStateChanged:
			ss << " : " << GetGameStateAsString() << "\n";
			break;
		case NewGame:
			ss << " : " << GetIPAdress() << " : " << GetPort() <<  "\n";
			break;
		case PlayerName:
			ss << " : " << GetPlayerName() << "\n";
			break;
		case BonusPickup:
			ss << " : " << xPos << " , " << yPos << " , " << xDir << " , " << yDir << "\n";
			break;
		case BulletFire:
			ss << " : " << xPos << " , " << yPos << " , " << xPos2 << " , " << yPos2 << "\n";
			break;
		default:
			ss << " : "  << xPos << " , " << yPos << " , " << xDir << " , " << yDir << "\n";
			break;
	}

	return ss.str();
}
// Getters
std::string TCPMessage::GetTypeAsString() const
{
	switch ( msgType )
	{
		case PaddlePosition:
			return "Paddle Position";
		case BallSpawned:
			return "Ball Spawned";
		case BallData:
			return "Ball Data";
		case BallKilled:
			return "Ball Killed";
		case TileHit:
			return "Tile Hit";
		case GameSettings:
			return "Game Settings";
		case GameStateChanged:
			return "Game State Changed";
		case BonusSpawned:
			return "Bonus Spawned";
		case BonusPickup:
			return "Bonus Pickup";
		case NewGame:
			return "New Game";
		case PlayerName:
			return "Player Name";
		case GetGameList:
			return "Get Game List";
		case EndGame:
			return "End Game";
		case GameJoined:
			return "Game Joined";
		case BulletFire:
			return "Bullet Fire";
		case LevelDone:
			return "Level Done";
		default:
			return "Unknown";
	}
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
std::string TCPMessage::GetBonusTypeAsString() const
{
	switch ( bonusType )
	{
		case BonusType::ExtraLife:
			return "Extra Life";
		case BonusType::Death:
			return "Death";
		default:
			break;
	}

	return "Unknown";
}
int32_t TCPMessage::GetBonusTypeAsInt() const
{
	return static_cast< int32_t > ( bonusType );
}
BonusType TCPMessage::GetBonusType() const
{
	return bonusType;
}
std::string TCPMessage::GetGameStateAsString() const
{
	switch ( newGameState )
	{
		case GameState::GameOver:
			return "Game Oever";
		case GameState::InGame:
			return "In Game";
		case GameState::InGameMenu:
			return "In Game Menu";
		case GameState::InGameWait:
			return "In Game Wait";
		case GameState::Lobby:
			return "Lobby";
		case GameState::MainMenu:
			return "Main Menu";
		case GameState::Options:
			return "Options";
		case GameState::Paused:
			return "Paused";
		case GameState::Quit:
			return "Quit";
	}
}
GameState TCPMessage::GetGameState() const
{
	return newGameState;
}
int32_t TCPMessage::GetGameStateAsInt() const
{
	return static_cast< int32_t > ( newGameState );
}
double TCPMessage::GetXPos() const
{
	return xPos;
}
double TCPMessage::GetYPos() const
{
	return yPos;
}
double TCPMessage::GetXPos2() const
{
	return xPos2;
}
double TCPMessage::GetYPos2() const
{
	return yPos2;
}
double TCPMessage::GetXDir() const
{
	return xDir;
}
double TCPMessage::GetYDir() const
{
	return yDir;
}
double TCPMessage::GetXSize() const
{
	return xSize;
}
double TCPMessage::GetYSize() const
{
	return ySize;
}
double TCPMessage::GetBoardScale() const
{
	return boardScale;
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

void TCPMessage::SetBonusType( BonusType bonusType_ )
{
	bonusType = bonusType_;
}
void TCPMessage::SetBonusType( int32_t bonusType_ )
{
	bonusType = static_cast< BonusType > ( bonusType_ );
}
void TCPMessage::SetGameState( int32_t gameState_ )
{
	newGameState = static_cast< GameState > ( gameState_ );
}
void TCPMessage::SetGameState( GameState gameState_ )
{
	newGameState = gameState_;
}
void TCPMessage::SetXPos( double xPos_ )
{
	xPos = xPos_;
}
void TCPMessage::SetYPos( double yPos_ )
{
	yPos = yPos_;
}
void TCPMessage::SetXPos2( double xPos_ )
{
	xPos2 = xPos_;
}
void TCPMessage::SetYPos2( double yPos_ )
{
	yPos2 = yPos_;
}
void TCPMessage::SetXDir( double xDir_ )
{
	xDir = xDir_;
}
void TCPMessage::SetYDir( double yDir_ )
{
	yDir = yDir_;
}
void TCPMessage::SetXSize( double xSize_ )
{
	xSize = xSize_;
}
void TCPMessage::SetYSize( double ySize_ )
{
	ySize = ySize_;
}
void TCPMessage::SetBoardScale( double boardScale_)
{
	boardScale = boardScale_;
}
