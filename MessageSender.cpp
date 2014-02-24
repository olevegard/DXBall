#include "MessageSender.h"

MessageSender::MessageSender( NetManager &netMan )
:	netManager( netMan )
{
	logger.Init( "log_ms.txt" );
}
void MessageSender::SendBulletKilledMessage( uint32_t bulletID )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::BulletKilled );
	msg.SetObjectID(  bulletID );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendBulletFireMessage( const std::shared_ptr< Bullet > &bulletLeft, const std::shared_ptr< Bullet > &bulletRight, double height  )
{
	TCPMessage msg;
	std::stringstream ss;

	msg.SetMessageType( MessageType::BulletFire );
	msg.SetObjectID( bulletLeft->GetObjectID() );
	msg.SetObjectID2( bulletRight->GetObjectID() );

	msg.SetPos1( FlipPosition( bulletLeft->rect , height ));
	msg.SetPos2( FlipPosition( bulletRight->rect, height ));

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendBonusBoxSpawnedMessage( const std::shared_ptr< BonusBox > &bonusBox, double height )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::BonusSpawned );
	msg.SetObjectID( bonusBox->GetObjectID() );
	msg.SetBonusType( bonusBox->GetBonusType() );

	msg.SetPos1( FlipPosition( bonusBox->rect , height ));

	msg.SetDir( bonusBox->GetDirection_YFlipped() );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendBonusBoxPickupMessage( uint32_t boxID )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::BonusPickup );
	msg.SetObjectID( boxID );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendBallKilledMessage( uint32_t ballID )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::BallKilled );
	msg.SetObjectID(  ballID  );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendBallSpawnMessage( const std::shared_ptr<Ball> &ball, double height )
{
	TCPMessage msg;
	Rect r = ball->rect;

	msg.SetMessageType( MessageType::BallSpawned );
	msg.SetObjectID( ball->GetObjectID() );

	msg.SetPos1( FlipPosition( r , height ));
	msg.SetDir( ball->GetDirection_YFlipped()  );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendBallDataMessage( const std::shared_ptr<Ball> &ball, double height )
{
	TCPMessage msg;
	Rect r = ball->rect;

	msg.SetMessageType( MessageType::BallData );
	msg.SetObjectID( ball->GetObjectID()  );

	//msg.SetPos1( Vector2f( r.x, windowHeight - r.y  ) );
	msg.SetPos1( FlipPosition( r , height ));
	msg.SetDir( ball->GetDirection_YFlipped() );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendTileSpawnMessage( const std::shared_ptr<Tile> &tile, double height )
{
	TCPMessage msg;
	Rect r = tile->rect;

	msg.SetMessageType( MessageType::TileSpawned );
	msg.SetObjectID( tile->GetObjectID() );

	msg.SetTileType( tile->GetTileType() );
	msg.SetPos1( FlipPosition( r, height ));

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendTileHitMessage( uint32_t tileID, bool tileKilled )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::TileHit );
	msg.SetObjectID( tileID );
	msg.SetTileKilled( tileKilled );

	SendMessage( msg, MessageTarget::Oponent, true);
}
void MessageSender::SendLastTileMessage( )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::LastTileSent );
	msg.SetObjectID( 0 );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendLevelDoneMessage( )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::LevelDone );
	msg.SetObjectID( 0 );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendPaddlePosMessage( double xPos  )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::PaddlePosition );
	msg.SetPos1( Vector2f( xPos, 0 ) );

	std::stringstream ss;
	ss << msg;

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendPlayerName( const std::string &playerName )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::PlayerName );
	msg.SetPlayerName( playerName );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendGameSettingsMessage( const Vector2f &size, double scale )
{
	if ( !netManager.IsServer()  )
	{
		std::cout << "MEessageSEnder@" << __LINE__ << " failed to send : GameSettings" << std::endl;
		return;
	}

	TCPMessage msg;

	msg.SetMessageType( MessageType::GameSettings );
	msg.SetObjectID( 0 );

	msg.SetSize( size );

	msg.SetBoardScale( scale  );

	SendMessage( msg, MessageTarget::Oponent );
}
void MessageSender::SendGameStateChangedMessage( const GameState &gameState )
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::GameStateChanged );
	msg.SetObjectID( 0 );
	msg.SetGameState( gameState );

	SendMessage( msg, MessageTarget::Oponent );
}

void MessageSender::SendNewGameMessage( const std::string &ip, uint16_t port )
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::NewGame );
	msg.SetIPAdress( ip  );
	msg.SetPort( port );

	SendMessage( msg, MessageTarget::Server );
}
void MessageSender::SendJoinGameMessage( int32_t gameID )
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::GameJoined );
	msg.SetObjectID( gameID );

	SendMessage( msg, MessageTarget::Server );
}
void MessageSender::SendEndGameMessage( int32_t gameID, const std::string &ip, uint16_t port )
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::EndGame );

	msg.SetObjectID( gameID );
	msg.SetIPAdress( ip );
	msg.SetPort( port );

	SendMessage( msg, MessageTarget::Server );
}
void MessageSender::SendGetGameListMessage()
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::GetGameList );

	SendMessage( msg, MessageTarget::Server );;
}
void MessageSender::SendMessage( const TCPMessage &message, const MessageTarget &target, bool print )
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
void MessageSender::PrintSend( const TCPMessage &msg )
{
	std::cout << "Message sent : " << msg.Print() << std::endl;;
	//logger.Log( __FILE__, __LINE__, msg.Print() );
}
Vector2f MessageSender::FlipPosition( Rect originalPos, double height )
{
	return Vector2f( originalPos.x , height - originalPos.y  - originalPos.h );
}
