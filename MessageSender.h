#pragma once

#include <memory>
#include "NetManager.h"

#include "structs/net/TCPMessage.h"
#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Bullet.h"
#include "structs/game_objects/Paddle.h"
#include "structs/game_objects/BonusBox.h"

#include "enums/MessageTarget.h"
#include "enums/GameState.h"

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
	void SendTileHitMessage( uint32_t tileID );
	void SendLastTileMessage( );

	void SendNewGameMessage( const std::string &ip, uint16_t port );
	void SendJoinGameMessage( int32_t gameID );
	void SendEndGameMessage( int32_t gameID, const std::string &ip, uint16_t port );
	void SendGetGameListMessage();
private:
	void SendMessage( const TCPMessage &message, const MessageTarget &target, bool print = false );
	void PrintSend( const TCPMessage &msg ) const;
	Vector2f FlipPosition( Rect originalPos, double height );

	NetManager &netManager;
};
