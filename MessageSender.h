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
	MessageSender( NetManager &netMan )
		:	netManager( netMan )
	{

	}
	void SendBulletKilledMessage( uint32_t bulletID )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::BulletKilled );
		msg.SetObjectID(  bulletID );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendBulletFireMessage( const std::shared_ptr< Bullet > &bulletLeft, const std::shared_ptr< Bullet > &bulletRight, double height  )
	{
		TCPMessage msg;
		std::stringstream ss;

		msg.SetMessageType( MessageType::BulletFire );
		msg.SetObjectID( bulletLeft->GetObjectID() );
		msg.SetObjectID2( bulletRight->GetObjectID() );

		msg.SetPos( Vector2f( bulletLeft->rect.x, height - bulletLeft->rect.y ) );

		msg.SetXPos2( bulletRight->rect.x );
		msg.SetYPos2( height - bulletRight->rect.y );

		std::cout << "Sending : " << msg << std::endl;
		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendBonusBoxSpawnedMessage( const std::shared_ptr< BonusBox > &bonusBox, double windowHeight )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::BonusSpawned );
		msg.SetObjectID( bonusBox->GetObjectID() );
		msg.SetBonusType( bonusBox->GetBonusType() );

		msg.SetPos( Vector2f( bonusBox->rect.x, windowHeight - bonusBox->rect.y ) );

		msg.SetXDir( bonusBox->GetDirection().x );
		msg.SetYDir( bonusBox->GetDirection().y );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendBonusBoxPickupMessage( uint32_t boxID )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::BonusPickup );
		msg.SetObjectID( boxID );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendBallKilledMessage( uint32_t ballID )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::BallKilled );
		msg.SetObjectID(  ballID  );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendBallSpawnMessage( const std::shared_ptr<Ball> &ball, double windowHeight )
	{
		TCPMessage msg;
		Rect r = ball->rect;

		msg.SetMessageType( MessageType::BallSpawned );
		msg.SetObjectID( ball->GetObjectID() );

		msg.SetPos( Vector2f( r.x, windowHeight - r.y ));

		msg.SetXDir( ball->GetDirection().x );
		msg.SetYDir( ball->GetDirection().y * -1.0 );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendBallDataMessage( const std::shared_ptr<Ball> &ball, double windowHeight )
	{
		TCPMessage msg;
		Rect r = ball->rect;

		msg.SetMessageType( MessageType::BallData );
		msg.SetObjectID( ball->GetObjectID()  );

		msg.SetPos( Vector2f( r.x, windowHeight - r.y  ) );

		msg.SetXDir( ball->GetDirection().x );
		msg.SetYDir( ball->GetDirection().y * -1.0 );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendTileHitMessage( uint32_t tileID )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::TileHit );
		msg.SetObjectID( tileID );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendLevelDoneMessage( )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::LevelDone );
		msg.SetObjectID( 0 );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendPaddlePosMessage( double xPos  )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::PaddlePosition );
		msg.SetPos( Vector2f( xPos, 0 ) );

		std::stringstream ss;
		ss << msg;

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendPlayerName( const std::string &playerName )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::PlayerName );
		msg.SetPlayerName( playerName );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendGameSettingsMessage( double w, double h, double scale )
	{
		if ( !netManager.IsServer()  )
		{
			std::cout << "MEessageSEnder@" << __LINE__ << " failed to send : GameSettings" << std::endl;
			return;
		}

		TCPMessage msg;

		msg.SetMessageType( MessageType::GameSettings );
		msg.SetObjectID( 0 );

		msg.SetXSize( w );
		msg.SetYSize( h );

		msg.SetBoardScale( scale  );

		SendMessage( msg, MessageTarget::Oponent );
	}
	void SendGameStateChangedMessage( const GameState &gameState )
	{
		TCPMessage msg;

		msg.SetMessageType( MessageType::GameStateChanged );
		msg.SetObjectID( 0 );
		msg.SetGameState( gameState );

		SendMessage( msg, MessageTarget::Oponent );
	}

	void SendNewGameMessage( const std::string &ip, uint16_t port )
	{
		TCPMessage msg;
		msg.SetMessageType( MessageType::NewGame );
		msg.SetIPAdress( ip  );
		msg.SetPort( port );

		SendMessage( msg, MessageTarget::Server );
	}
	void SendJoinGameMessage( int32_t gameID )
	{
		TCPMessage msg;
		msg.SetMessageType( MessageType::GameJoined );
		msg.SetObjectID( gameID );

		SendMessage( msg, MessageTarget::Server );
	}
	void SendEndGameMessage( int32_t gameID, const std::string &ip, uint16_t port )
	{
		TCPMessage msg;
		msg.SetMessageType( MessageType::EndGame );

		msg.SetObjectID( gameID );
		msg.SetIPAdress( ip );
		msg.SetPort( port );

		SendMessage( msg, MessageTarget::Server );
	}
	void SendGetGameListMessage()
	{
		TCPMessage msg;
		msg.SetMessageType( MessageType::GetGameList );

		SendMessage( msg, MessageTarget::Server );;
	}
private:
	void SendMessage( const TCPMessage &message, const MessageTarget &target, bool print = false )
	{
		std::stringstream ss;
		ss << message;

		if ( target == MessageTarget::Oponent )
			netManager.SendMessage( ss.str() );
		else
			netManager.SendMessageToServer( ss.str() );

		if ( print )
			PrintSend( message );
	}
	void PrintSend( const TCPMessage &msg ) const
	{
		std::cout << "Message sent : " << msg.Print();
	}
	NetManager &netManager;
};
