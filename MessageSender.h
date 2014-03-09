#pragma once

#include <memory>
#include "NetManager.h"
#include "Logger.h"
/*

#include "enums/MessageTarget.h"
#include "enums/GameState.h"
*/


struct GamePiece;
struct BonusBox;
struct Bullet;
struct Paddle;
struct Ball;
struct Tile;
struct Vector2f;
struct Rect;
class TCPMessage;
enum class MessageTarget : int;
enum class GameState : int;
enum class TileType : int;
class MessageSender
{
public:
	MessageSender( NetManager &netMan );

	void SendBulletKilledMessage( uint32_t bulletID );
	void SendBulletFireMessage( const std::shared_ptr< Bullet > &bulletLeft, const std::shared_ptr< Bullet > &bulletRight, double height  );
	void SendBonusBoxSpawnedMessage( const std::shared_ptr< BonusBox > &bonusBox, double height );
	void SendBonusBoxPickupMessage( uint32_t boxID );
	void SendBallKilledMessage( uint32_t ballID );
	void SendBallSpawnMessage( const std::shared_ptr<Ball> &ball, double height );
	void SendBallDataMessage( const std::shared_ptr<Ball> &ball, double height );
	void SendLevelDoneMessage( );
	void SendPaddlePosMessage( double xPos  );
	void SendPlayerName( const std::string &playerName );
	void SendGameSettingsMessage( const Vector2f &size, double scale );
	void SendGameStateChangedMessage( const GameState &gameState );

	void SendTileSpawnMessage( const std::shared_ptr<Tile> &tile, double height );
	void SendTileHitMessage( uint32_t tileID, bool tileKilled = false );
	void SendLastTileMessage( );

	void SendNewGameMessage( const std::string &ip, uint16_t port );
	void SendJoinGameMessage( int32_t gameID );
	void SendEndGameMessage( int32_t gameID, const std::string &ip, uint16_t port );
	void SendGetGameListMessage();
private:
	void SendMessage( const TCPMessage &message, const MessageTarget &target, bool print = false );
	void PrintSend( const TCPMessage &msg );
	Vector2f FlipPosition( Rect originalPos, double height );

	NetManager &netManager;
	Logger logger;
};
