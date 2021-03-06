#include "TCPMessage.h"

TCPMessage::TCPMessage()
	:	msgType( MessageType::PaddlePosition )
	,	objectID( 0 )
{}
std::string TCPMessage::Print() const
{
	std::stringstream ss;
	ss << "Type : " << std::left << std::setw( 10 ) << GetTypeAsString() << "| Object ID : "  << std::left << std::setw( 4 ) << objectID;

	switch ( msgType )
	{
		case EndGame:
		case LevelDone:
		case GameJoined:
		case BallKilled:
		case GetGameList:
		case LastTileSent:
			break;
		case TileHit:
			ss << " : Killed ? " << std::boolalpha << tileKilled;
			break;
		case PaddlePosition:
			ss << " : "  << pos1.x;
			break;
		case TileSpawned:
			ss << " : "  << pos1 << " Type : " << GetTileTypeAsString();
			break;
		case GameSettings:
			ss << " : "  << size << " board scale : " << boardScale;
			break;
		case BonusSpawned:
			ss << " : " << GetBonusTypeAsString() << " , "  << pos1 << " , " << dir;
			break;
		case GameStateChanged:
			ss << " : " << GetGameStateAsString();
			break;
		case NewGame:
			ss << " : " << GetIPAdress() << " : " << GetPort();
			break;
		case PlayerName:
			ss << " : " << GetPlayerName();
			break;
		case BonusPickup:
			ss << " : " << pos1 << " , " << dir;
			break;
		case BulletFire:
			ss << " : " << pos1 << " Object 2 ID : " << objectID2 << " Pos : " << pos2;
			break;
		default:
			ss << " : "  << pos1  << " , " <<  dir;
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
		case LastTileSent:
			return "Last Tile Sent";
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
unsigned int TCPMessage::GetObjectID2() const
{
	return objectID2;
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
TileType TCPMessage::GetTileType() const
{
	return tileType;
}
std::string TCPMessage::GetTileTypeAsString() const
{
	switch ( tileType )
	{
		case TileType::Regular:
			return "Regular";
		case TileType::Explosive:
			return "Explosive";
		case TileType::Hard:
			return "Hard";
		case TileType::Unbreakable:
			return "Unbreakable";
		case TileType::Wall_Of_Death:
			return "Wall_Of_Death";
		//default: return "Unknown";
	}
}
bool TCPMessage::GetTileKilled() const
{
	return tileKilled;
}
GameState TCPMessage::GetGameState() const
{
	return newGameState;
}
int32_t TCPMessage::GetGameStateAsInt() const
{
	return static_cast< int32_t > ( newGameState );
}
double TCPMessage::GetBoardScale() const
{
	return boardScale;
}
Vector2f TCPMessage::GetDir() const
{
	return dir;
}
Vector2f TCPMessage::GetDir_YFlipped() const

{
	return Vector2f( dir.x, dir.y * -1.0 );
}
Vector2f TCPMessage::GetPos1() const
{
	return pos1;
}
Vector2f TCPMessage::GetPos2() const
{
	return pos2;
}
Vector2f TCPMessage::GetSize() const
{
	return size;
}
uint16_t TCPMessage::GetPort() const
{
	return port;
}
std::string TCPMessage::GetIPAdress() const
{
	return ipAddress;
}
std::string TCPMessage::GetPlayerName() const
{
	return playerName;
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
void TCPMessage::SetObjectID2( unsigned int objectID_ )
{
	objectID2 = objectID_;
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
void TCPMessage::SetBoardScale( double boardScale_)
{
	boardScale = boardScale_;
}
void TCPMessage::SetPort( uint16_t port_ )
{
	port = port_;
}
void TCPMessage::SetIPAdress( std::string  ipAddress_ )
{
	ipAddress = ipAddress_;
}
void TCPMessage::SetPlayerName( std::string playerName_ )
{
	playerName = playerName_;
}
void TCPMessage::SetPos1( Vector2f pos )
{
	pos1 = pos;
}
void TCPMessage::SetPos2( Vector2f pos )
{
	pos2 = pos;
}
void TCPMessage::SetDir( Vector2f dir_ )
{
	dir = dir_;
}
void TCPMessage::SetSize( Vector2f size_ )
{
	size = size_;
}
void TCPMessage::SetTileType( TileType tileType_ )
{
	tileType = tileType_;
}
void TCPMessage::SetTileKilled( bool isKilled )
{
	tileKilled = isKilled;
}
