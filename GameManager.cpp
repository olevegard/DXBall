#include "GameManager.h"

#include "Ball.h"
#include "Tile.h"
#include "Board.h"
#include "Paddle.h"
#include "Bullet.h"
#include "BoardLoader.h"

#include "math/Math.h"
#include "math/RectHelpers.h"

#include "TCPMessage.h"

#include "enums/MessageType.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MessageTarget.h"

#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <cmath>

#include "ConfigLoader.h"


	GameManager::GameManager()
	:	runGame( true )
	,	renderer()
	,	timer()

	,	gameID( 0 )
	,	localPaddle()

	,	isAIControlled( false )
	,	localPlayerPoints( 0 )
	,	localPlayerLives( 3 )
	,	localPlayerActiveBalls( 0 )
	,	localPlayerSuperBall( false )

	,	remotePlayerPoints( 0 )
	,	remotePlayerLives( 3 )
	,	remotePlayerActiveBalls( 0 )
	,	remotePlayerSuperBall( false )

	,	ballList()
	,	windowSize()
	,	scale( 1.0 )
	,	points{ 20, 50, 100, 500 }

	,	remoteResolutionScale( 1.0 )
	,	isResolutionScaleRecieved( false )

	,	tileCount( 0 )
	,	ballCount( 0 )
	,	bonusCount( 0 )

	,	fpsLimit( 60 )
	,	frameDuration( 1000.0 / 60.0 )
	,	ballSpeed( 0.2 )
	,	bonusBoxSpeed( 0.2 )
{
	windowSize.x = 0.0;
	windowSize.y = 0.0;
	windowSize.w = 1920 / 2;
	windowSize.h = 1080 / 2;
}

bool GameManager::Init( const std::string &localPlayerName_,  const SDL_Rect &size, bool startFS )
{
	localPlayerName = localPlayerName_;

	windowSize = size;
	bool server = localPlayerName ==  "server";

	if ( !renderer.Init( windowSize, startFS, server ) )
		return false;

	UpdateGUI();

	renderer.RenderPlayerCaption( localPlayerName, Player::Local );


	localPaddle = std::make_shared< Paddle > ();
	localPaddle->textureType = TextureType::e_Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h - ( localPaddle->rect.h * 1.5 );
	localPaddle->SetScale( scale );

	renderer.SetLocalPaddle( localPaddle );
	menuManager.Init( renderer );
	CreateMenu();

	ConfigLoader cfg;
	cfg.LoadConfig();
	bonusBoxSpeed = cfg.GetBonusBoxSpeed();
	ballSpeed = cfg.GetBallSpeed();


	return true;
}

void GameManager::InitNetManager( std::string ip_, uint16_t port_ )
{
	ip = ip_;
	port = port_;
	std::cout << "IP : " << ip << " | Port : " << port << "\n";
	netManager.Init( true  );

	GameInfo gameInfo;
	gameInfo.Set( ip, port );
	//menuManager.AddGameToList( renderer, hostInfo );
}
void GameManager::Restart()
{
	if ( menuManager.IsTwoPlayerMode() )
	{
		remotePaddle = std::make_shared< Paddle > ();
		remotePaddle->textureType = TextureType::e_Paddle;
		remotePaddle->rect.w = 120;
		remotePaddle->rect.h = 30;
		remotePaddle->rect.x = 400;

		remotePaddle->rect.y = remotePaddle->rect.h * 0.5;
		remotePaddle->SetScale( scale );

		renderer.SetRemotePaddle( remotePaddle );
	}
	isResolutionScaleRecieved = false;

	tileCount = 0;
	ballCount = 0;

	boardLoader.Reset();

	if ( !menuManager.IsTwoPlayerMode() )
	{
		boardLoader.SetIsServer( true );
	}

	if ( !menuManager.IsTwoPlayerMode() || netManager.IsServer() )
		GenerateBoard();

	localPlayerPoints = 0;
	localPlayerLives = 3;
	localPlayerActiveBalls = 0;
	localPlayerSuperBall = false;

	remotePlayerPoints = 0;
	remotePlayerLives = 3;
	remotePlayerActiveBalls = 0;
	remotePlayerSuperBall = false;

	renderer.SetIsTwoPlayerMode( menuManager.IsTwoPlayerMode() );
	renderer.ResetText();

	UpdateGUI();
}

std::shared_ptr<Ball> GameManager::AddBall( Player owner, unsigned int ballID )
{
	if ( menuManager.GetGameState() != GameState::InGame )
		return nullptr;

	if ( owner == Player::Local )
	{
		if (  localPlayerLives == 0 )
		{
			return nullptr;
		}
		if ( localPlayerActiveBalls == 0 )
			renderer.StartFade();

		++localPlayerActiveBalls;
	}
	else  if ( owner == Player::Remote )
	{
		if (  remotePlayerLives == 0 )
		{
			return nullptr;
		}
		++remotePlayerActiveBalls;
	}

	std::shared_ptr< Ball > ball = std::make_shared< Ball >( windowSize, owner, ballID );
	ball->textureType = TextureType::e_Ball;

	ball->SetScale( scale );
	ball->SetSpeed( ballSpeed );

	ballList.push_back( ball );
	renderer.AddBall( ball );

	if ( owner == Player::Local )
		SendBallSpawnMessage( ball );

	++ballCount;

	return ball;
}

void GameManager::RemoveBall( const std::shared_ptr< Ball >  ball )
{
	if ( !ball )
		return;

	if ( ball->GetOwner() == Player::Local )
	{
		--localPlayerActiveBalls;

		SendBallKilledMessage( ball );
	}
	else
	{
		--remotePlayerActiveBalls;

		renderer.RenderBallCount( remotePlayerActiveBalls, Player::Remote );
	}

	if ( localPlayerActiveBalls == 0 && ball->GetOwner() == Player::Local )
			ReducePlayerLifes( Player::Local );

	if ( remotePlayerActiveBalls == 0 && ball->GetOwner() == Player::Remote )
			ReducePlayerLifes( Player::Remote );

	renderer.RemoveBall( ball );
	UpdateGUI();
}
void GameManager::AddTile( short posX, short posY, TileType tileType )
{
	std::shared_ptr< Tile > tile = std::make_shared< Tile >( tileType, tileCount++ );
	tile->textureType = TextureType::e_Tile;

	tile->rect.x = posX;
	tile->rect.y = posY;
	tile->rect.w = 60 * scale;
	tile->rect.h = 20 * scale;

	tileList.push_back( tile );

	renderer.AddTile( tile );
}
void GameManager::RemoveTile( std::shared_ptr< Tile > tile )
{
	if ( tile == nullptr )
		return;

	renderer.RemoveTile( tile );

	// Decrement tile count
	--tileCount;
}

void GameManager::AddBonusBox( const std::shared_ptr< Ball > &triggerBall, double x, double y, int tilesDestroyed /* = 1 */ )
{
	int randMax = 10;
	if ( tilesDestroyed != 1 )
	{
		double probabilityOfNoBonus = std::pow( 0.99, tilesDestroyed * 2);

		randMax = static_cast< int > ( probabilityOfNoBonus * 100 );
	}

	if ( Math::GenRandomNumber( ( randMax > 0 ) ? randMax : 1 ) != 1 )
		return;

	std::shared_ptr< BonusBox > bonusBox  = std::make_shared< BonusBox > ( bonusCount++ );
	bonusBox->rect.x = x;
	bonusBox->rect.y = y;

	Player ballOwner = triggerBall->GetOwner();

	Vector2f direction = triggerBall->GetDirection();

	// Force correct y dir
	if ( ballOwner == Player::Local )
		direction.y = ( direction.y > 0.0 ) ? direction.y : direction.y * -1.0;
	else
		direction.y = ( direction.y < 0.0 ) ? direction.y : direction.y * -1.0;

	bonusBox->SetDirection( direction );
	bonusBox->SetOwner( ballOwner  );
	bonusBox->SetBonusType( GetRandomBonusType()  );
	bonusBox->SetSpeed( bonusBoxSpeed );
	std::cout << "Setting speed to : " << bonusBoxSpeed << std::endl;

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
	SendBonusBoxSpawnedMessage( bonusBox );
}
void GameManager::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
{
	renderer.RemoveBonusBox( bb );
}
void GameManager::UpdateBalls( double delta )
{
	for ( auto p : ballList )
	{
		p->Update( delta );

		if ( p->GetOwner() == Player::Remote )
			continue;

		if ( p->BoundCheck( windowSize ) || p->PaddleCheck( localPaddle->rect )  )
		{
			SendBallDataMessage( p );
			continue;
		}

		CheckBallTileIntersection( p );

		if ( p->DeathCheck( windowSize ) )
			p->Kill();
	}

	DeleteDeadBalls();
}
void GameManager::UpdateBullets( double delta )
{
	///for ( auto bullet : bulletList )

	std::vector< std::vector< std::shared_ptr< Bullet > >::iterator > deadBulletList;

	for ( auto bullet = bulletList.begin() ; bullet != bulletList.end() ; ++bullet  )
	{
		(*bullet)->Update( delta );

		for ( auto tile = tileList.begin() ; tile != tileList.end() ;  )
		{
			double tileLeft =   (*tile)->rect.x;
			double tileTop =    (*tile)->rect.y;
			double tileRight =  (*tile)->rect.x + (*tile)->rect.w;
			double tileBottom = (*tile)->rect.y + (*tile)->rect.h;

			double ballLeft =   (*bullet)->rect.x;
			double ballTop =    (*bullet)->rect.y;
			double ballRight =  (*bullet)->rect.x + (*bullet)->rect.w;
			double ballBottom = (*bullet)->rect.y + (*bullet)->rect.h;

			// Intersection test
			if  (!(
					ballTop    > tileBottom
					|| ballLeft   > tileRight
					|| ballRight  < tileLeft
					|| ballBottom < tileTop
					))
			{
				(*tile)->Hit();
				bool isDestroyed = (*tile)->IsDestroyed();
				IncrementPoints( (*tile)->GetTileTypeAsIndex(), isDestroyed, Player::Local );

				deadBulletList.push_back( bullet );

				//if ( (*tile)->GetTileType() == TileType::Explosive ) HandleExplosions( (*tile), Player::Local );

				if ( isDestroyed )
				{
					RemoveTile( (*tile ) );
					tile = tileList.erase( tile );
					continue;
				}
				else
					++tile;
			}
			else
			{
				++tile;
			}
		}
	}

	for ( auto p : deadBulletList )
	{
		renderer.RemoveBullet( (*p ));
		bulletList.erase( p );
	}

	//bullet = bulletList.erase( bullet );
}
void GameManager::UpdateNetwork()
{
	ReadMessagesFromServer();

	if ( !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() )
		return;

	if ( !isResolutionScaleRecieved )
	{
		SendGameSettingsMessage();
		SendPlayerName();
	}

	ReadMessages();
}
void GameManager::ReadMessages( )
{
	TCPMessage msg;
	while ( true  )
	{
		std::string str = netManager.ReadMessage();

		if ( str == "" )
			break;

		std::stringstream ss;
		ss << str;

		while ( ss >> msg )
		{
			HandleRecieveMessage( msg );
		}
	}
}
void GameManager::ReadMessagesFromServer( )
{
	TCPMessage msg;
	while ( true  )
	{
		std::string str = netManager.ReadMessageFromServer();

		if ( str == "" )
			break;

		std::stringstream ss;
		ss << str;

		while ( ss >> msg )
		{
			HandleRecieveMessage( msg );
		}
	}
}
void GameManager::HandleRecieveMessage( const TCPMessage &message )
{
	switch ( message.GetType() )
	{
		case MessageType::GameSettings:
			RecieveGameSettingsMessage( message );
			break;
		case MessageType::GameStateChanged:
			RecieveGameStateChangedMessage( message );
			break;
		case MessageType::PaddlePosition:
			RecievePaddlePosMessage( message );
			break;
		case MessageType::BallSpawned:
			RecieveBallSpawnMessage( message );
			break;
		case MessageType::BallData:
			RecieveBallDataMessage( message );
			break;
		case MessageType::BallKilled:
			RecieveBallKillMessage( message );
			break;
		case MessageType::TileHit:
			RecieveTileHitMessage( message );
			break;
		case MessageType::BonusSpawned:
			RecieveBonusBoxSpawnedMessage( message );
			break;
		case MessageType::BonusPickup:
			RecieveBonusBoxPickupMessage( message );
			break;
		case MessageType::PlayerName:
			RecievePlayerNameMessage( message );
			break;
		case MessageType::NewGame:
			RecieveNewGameMessage( message );
			break;
		case MessageType::GameJoined:
			RecieveJoinGameMessage( message );
			break;
		case MessageType::EndGame:
			RecieveEndGameMessage( message );
			break;
		default:
			std::cout << "GameManager@" << __LINE__ << " : UpdateNetwork message received " << message << std::endl;
			std::cin.ignore();
			break;
	}
}

void GameManager::RecieveJoinGameMessage( const TCPMessage &message  )
{
	UpdateGameList();

	PrintRecv( message );
}
void GameManager::RecieveNewGameMessage( const TCPMessage &message )
{
	GameInfo info;
	info.Set( message.GetIPAdress(), message.GetPort() );
	info.SetGameID( message.GetObjectID() );
	menuManager.AddGameToList( renderer, info );

	PrintRecv( message );
}
void GameManager::RecieveEndGameMessage( const TCPMessage &message )
{
	UpdateGameList();
	menuManager.ClearGameList();

	PrintRecv( message );
}
void GameManager::RecievePlayerNameMessage( const TCPMessage &message )
{
	PrintRecv( message );
	renderer.RenderPlayerCaption( message.GetPlayerName(), Player::Remote );
}
void GameManager::RecieveGameSettingsMessage( const TCPMessage &message)
{
	remoteResolutionScale = windowSize.w / message.GetXSize();
	PrintRecv( message );

	if ( !netManager.IsServer() || !menuManager.IsTwoPlayerMode()  )
	{
		GenerateBoard();
		SetScale( message.GetBoardScale() * remoteResolutionScale );
	}

	isResolutionScaleRecieved = true;
}
void GameManager::RecieveGameStateChangedMessage( const TCPMessage &message)
{
	menuManager.SetGameState( message.GetGameState() );
	PrintRecv( message );
}
void GameManager::RecieveBallSpawnMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	std::shared_ptr< Ball > ball = AddBall( Player::Remote, message.GetObjectID() );

	ball->rect.x = message.GetXPos() * remoteResolutionScale;
	ball->rect.y = message.GetYPos() * remoteResolutionScale;

	ball->SetDirection( Vector2f( message.GetXDir(), message.GetYDir() ) );
	ball->SetRemoteScale( remoteResolutionScale );
}
void GameManager::RecieveBallDataMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	if ( ballList.size() == 0 )
		return;

	std::shared_ptr< Ball > ball = GetBallFromID( message.GetObjectID() );

	if ( !ball )
	{
		std::cout << "GameManager@" << __LINE__ << " Could not find ball with ID : " << message.GetObjectID() << std::endl;
		return;
	}

	// Need to add ball's height, because ball it traveling in oposite direction.
	// The board is also flipped, so the ball will have the oposite horizontal collision edge.
	ball->rect.x = message.GetXPos() * remoteResolutionScale;
	ball->rect.y = ( message.GetYPos()  * remoteResolutionScale ) - ball->rect.h ;

	ball->SetDirection( Vector2f( message.GetXDir(), message.GetYDir() ) );
}
void GameManager::RecieveBallKillMessage( const TCPMessage &message )
{
	std::cout << "GameManager.cpp@" << __LINE__ << "Erasing ball kill message: \n";
	PrintRecv( message );
	if ( ballList.size() > 0 )
	{
		GetBallFromID( message.GetObjectID() )->Kill();
	}

	DeleteDeadBalls();
}
void GameManager::RecieveTileHitMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	if ( tileList.size() == 0 )
		return;

	std::shared_ptr< Tile > tile = GetTileFromID( message.GetObjectID() );

	tile->Hit();
	bool isDestroyed = tile->IsDestroyed();
	IncrementPoints( tile->GetTileTypeAsIndex(), isDestroyed, Player::Remote );

	if ( tile->GetTileType() == TileType::Explosive )
	{
		HandleExplosions( tile, Player::Remote );
		return;
	}

	if ( isDestroyed )
	{
		auto itClosestTile = std::find( tileList.begin(), tileList.end(), tile );
		tileList.erase( itClosestTile );
		RemoveTile( tile );
	}
}
void GameManager::RecievePaddlePosMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	if ( !remotePaddle )
		return;
	double xPos = message.GetXPos();
	if ( xPos > 0 && xPos < windowSize.w )
	{
		remotePaddle->rect.x = xPos;
	}
}
void GameManager::RecieveBonusBoxSpawnedMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	std::shared_ptr< BonusBox > bonusBox = std::make_shared< BonusBox >( message.GetObjectID()  );

	bonusBox->rect.x = message.GetXPos() * remoteResolutionScale;
	bonusBox->rect.y = ( message.GetYPos() * remoteResolutionScale ) - bonusBox->rect.h;

	bonusBox->SetOwner( Player::Remote );
	bonusBox->SetDirection( Vector2f( message.GetXDir(), message.GetYDir() * -1.0 ) );
	bonusBox->SetBonusType( message.GetBonusType() );
	bonusBox->SetSpeed( bonusBoxSpeed );

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
}
void GameManager::RecieveBonusBoxPickupMessage( const TCPMessage &message )
{
	PrintRecv( message );
	auto bb = GetBonusBoxFromID( message.GetObjectID() );

	if ( bb )
		ApplyBonus( bb );
}
void GameManager::SendPaddlePosMessage( )
{
	if ( !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::PaddlePosition );
	msg.SetXPos( localPaddle->rect.x );

	SendMessage( msg, MessageTarget::Oponent );
	//PrintSend(msg);
}

void GameManager::SendGameSettingsMessage()
{
	if ( !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() || !netManager.IsServer()  )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::GameSettings );
	msg.SetObjectID( 0 );

	msg.SetXSize( windowSize.w );
	msg.SetYSize( windowSize.h );

	msg.SetBoardScale( scale  );

	SendMessage( msg, MessageTarget::Oponent );

	isResolutionScaleRecieved  = true;
	PrintSend(msg);
}
void GameManager::SendPlayerName()
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::PlayerName );
	msg.SetPlayerName( localPlayerName );

	SendMessage( msg, MessageTarget::Oponent );

	PrintSend(msg);
}
void GameManager::SendBallSpawnMessage( const std::shared_ptr<Ball> &ball)
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	Rect r = ball->rect;

	msg.SetMessageType( MessageType::BallSpawned );
	msg.SetObjectID( ball->GetObjectID() );

	msg.SetXPos( r.x );
	msg.SetYPos( windowSize.h - r.y );

	msg.SetXDir( ball->GetDirection().x );
	msg.SetYDir( ball->GetDirection().y * -1.0 );

	SendMessage( msg, MessageTarget::Oponent );

	PrintSend(msg);
}
void GameManager::SendBallDataMessage( const std::shared_ptr<Ball> &ball)
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	Rect r = ball->rect;

	msg.SetMessageType( MessageType::BallData );
	msg.SetObjectID( ball->GetObjectID()  );

	msg.SetXPos( r.x );
	msg.SetYPos(  windowSize.h - r.y );

	msg.SetXDir( ball->GetDirection().x );
	msg.SetYDir( ball->GetDirection().y * -1.0 );

	SendMessage( msg, MessageTarget::Oponent );

	//PrintSend(msg);
}
void GameManager::SendBallKilledMessage( const std::shared_ptr<Ball> &ball)
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::BallKilled );
	msg.SetObjectID(  ball->GetObjectID() );

	SendMessage( msg, MessageTarget::Oponent );

	//PrintSend(msg);
}
void GameManager::SendTileHitMessage( unsigned int tileID )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::TileHit );
	msg.SetObjectID( tileID );

	SendMessage( msg, MessageTarget::Oponent );
}
void GameManager::SendBonusBoxSpawnedMessage( const std::shared_ptr< BonusBox > &bonusBox )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::BonusSpawned );
	msg.SetObjectID( bonusBox->GetObjectID() );
	msg.SetBonusType( bonusBox->GetBonusType() );

	msg.SetXPos( bonusBox->rect.x );
	msg.SetYPos( windowSize.h - bonusBox->rect.y );

	msg.SetXDir( bonusBox->GetDirection().x );
	msg.SetYDir( bonusBox->GetDirection().y );

	SendMessage( msg, MessageTarget::Oponent );

	//PrintSend( msg );
}
void GameManager::SendBonusBoxPickupMessage( const std::shared_ptr< BonusBox > &bonusBox )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;
	std::stringstream ss;

	msg.SetMessageType( MessageType::BonusPickup );
	msg.SetObjectID( bonusBox->GetObjectID() );

	ss << msg;
	netManager.SendMessage( ss.str() );

	PrintSend( msg );
}
void GameManager::SendGameStateChangedMessage()
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::GameStateChanged );
	msg.SetObjectID( 0 );
	msg.SetGameState( menuManager.GetGameState() );

	SendMessage( msg, MessageTarget::Oponent );
	PrintSend( msg );
}
void GameManager::SendNewGameMessage( )
{
	//if ( !netManager.IsServer() || !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() ) return;

	TCPMessage msg;
	msg.SetMessageType( MessageType::NewGame );
	msg.SetIPAdress( ip  );
	msg.SetPort( port );

	SendMessage( msg, MessageTarget::Server );
}
void GameManager::SendJoinGameMessage( const GameInfo &gameInfo )
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::GameJoined );
	msg.SetObjectID( gameInfo.GetGameID() );

	SendMessage( msg, MessageTarget::Server );
}
void GameManager::SendEndGameMessage( )
{
	//if ( !netManager.IsServer() || !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() ) return;

	TCPMessage msg;
	msg.SetMessageType( MessageType::EndGame );

	msg.SetObjectID( gameID );
	msg.SetIPAdress( ip );
	msg.SetPort( port );

	SendMessage( msg, MessageTarget::Server );
	PrintSend( msg );

}
void GameManager::SendGetGameListMessage()
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::GetGameList );

	std::stringstream ss;
	ss << msg;
	netManager.SendMessageToServer( ss.str() );

	PrintSend( msg );
}
void GameManager::SendMessage( const TCPMessage &message, const MessageTarget &target )
{
	std::stringstream ss;
	ss << message;

	if ( target == MessageTarget::Oponent )
		netManager.SendMessage( ss.str() );
	else
		netManager.SendMessageToServer( ss.str() );
}
void GameManager::PrintSend( const TCPMessage &msg ) const
{
	std::cout << "Sending : " << msg.Print();
}
void GameManager::PrintRecv( const TCPMessage &msg ) const
{
	std::cout << "Rceving : " << msg.Print();
}
void GameManager::DeleteDeadBalls()
{
	bool localPlayerBallDeleted = false;
	bool remotePlayerBallDeleted = false;
	auto isDeadFunc = [&]( std::shared_ptr< Ball > ball )
	{
		if ( !ball || ball->IsAlive() )
			return false;

		if ( ball->GetOwner() == Player::Local )
			localPlayerBallDeleted = true;
		else
			remotePlayerBallDeleted = true;

		RemoveBall( ball );
		return true;
	};

	auto newEnd = std::remove_if( ballList.begin(), ballList.end(), isDeadFunc );

	// Remove item returned by remove_if
	ballList.erase( newEnd, ballList.end( ) );
}
std::shared_ptr< Ball > GameManager::GetBallFromID( int32_t ID )
{
	for ( auto p : ballList )
	{
		if ( ID == p->GetObjectID() )
		{
			return p;
		}
	}

	return nullptr;
}
std::shared_ptr< Tile > GameManager::GetTileFromID( int32_t ID )
{
	for ( auto p : tileList )
	{
		if ( ID == p->GetObjectID() )
			return p;
	}

	return nullptr;
}

std::shared_ptr< BonusBox > GameManager::GetBonusBoxFromID( int32_t ID )
{
	for ( auto p : bonusBoxList )
	{
		if ( ID == p->GetObjectID() )
		{
			return p;
		}
	}

	return nullptr;

}
void GameManager::Run()
{
	unsigned int ticks;
	while ( runGame )
	{
		ticks = SDL_GetTicks();

		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			HandleEvent( event );
		}

		HandleStatusChange();

		Update( timer.GetDelta( ) );

		DoFPSDelay( ticks );
	}
}
void GameManager::HandleStatusChange( )
{
	if ( !menuManager.HasGameStateChanged() )
		return;

	SendGameStateChangedMessage();
	renderer.SetGameState( menuManager.GetGameState() );

	if ( menuManager.GetGameState() == GameState::Quit )
	{
		runGame = false;
	}
	else if ( menuManager.GetGameState() == GameState::Lobby )
	{
		UpdateGameList();
	}
	else if ( menuManager.WasGameStarted()  )
	{
		Restart();
	}
	else if ( menuManager.WasGameQuited() )
	{
		SendEndGameMessage();
		netManager.Close();
	}
}
void GameManager::HandleEvent( const SDL_Event &event )
{
	if ( event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP )
	{
		HandleMouseEvent( event.button );
	} else if ( event.type == SDL_WINDOWEVENT)
	{
		//if ( SDL_WINDOWEVENT_LEAVE ) renderer.ForceMouseFocus();
	} else if ( event.type == SDL_QUIT )
	{
		menuManager.SetGameState( GameState::Quit );
	} else if ( event.type == SDL_KEYDOWN )
	{
		HandleKeyboardEvent( event );
	}
}
void GameManager::HandleMouseEvent(  const SDL_MouseButtonEvent &buttonEvent )
{
	if ( menuManager.GetGameState() == GameState::InGame && !isAIControlled )
	{
		SetLocalPaddlePosition( buttonEvent.x, buttonEvent.y );
	}
	else if ( menuManager.IsInAMenu() )
	{
		if ( buttonEvent.type == SDL_MOUSEBUTTONDOWN )
		{
			menuManager.CheckItemMouseClick( buttonEvent.x, buttonEvent.y );
		}
		else
		{
			menuManager.CheckItemMouseOver( buttonEvent.x, buttonEvent.y, renderer );
		}
	}
}
void GameManager::HandleKeyboardEvent( const SDL_Event &event )
{
	HandleMenuKeys( event );
	HandleGameKeys( event );
}
void GameManager::HandleMenuKeys( const SDL_Event &event )
{
	if ( event.type == SDL_KEYDOWN )
	{
		switch  ( event.key.keysym.sym )
		{
			case SDLK_F11:
				renderer.ToggleFullscreen();
				break;
			case SDLK_ESCAPE:
				menuManager.GoToMenu();
				break;
			case SDLK_s:
				menuManager.SetGameState( GameState::InGame );
				break;
			case SDLK_m:
				menuManager.SetGameState( GameState::Lobby );
				break;
			case SDLK_q:
				menuManager.SetGameState( GameState::Quit );
				break;
			case SDLK_p:
				menuManager.SetGameState( GameState::Paused );
				break;
			case SDLK_r:
				menuManager.SetGameState( GameState::InGame );
				break;
			default:
				break;
		}
	}

}
void GameManager::HandleGameKeys( const SDL_Event &event )
{
	if ( menuManager.GetGameState() != GameState::InGame || ( menuManager.IsTwoPlayerMode() && !isResolutionScaleRecieved ) )
		return;

	if ( event.type == SDL_KEYDOWN )
	{
		switch  ( event.key.keysym.sym )
		{
			case SDLK_a:
				SetAIControlled( !isAIControlled );
				break;
			case SDLK_RETURN:
			case SDLK_b:
				//AddBall( Player::Remote );
				AddBall( Player::Local, ballCount );
				break;
			case SDLK_f:
				{
					std::shared_ptr< Bullet > bullet = std::make_shared< Bullet >( 0 );
					bullet->SetPosition( localPaddle->rect.x, localPaddle->rect.y - 10 );
					bulletList.push_back( bullet );
					renderer.AddBullet( bullet );

					std::shared_ptr< Bullet > bullet2 = std::make_shared< Bullet >( 0 );
					bullet2->SetPosition( localPaddle->rect.x + localPaddle->rect.w, localPaddle->rect.y - 10 );
					bulletList.push_back( bullet2 );
					renderer.AddBullet( bullet2 );
				}
				break;
			case SDLK_u:
				ResetScale();
				break;
			default:
				break;
		}
	}
}
void GameManager::DoFPSDelay( unsigned int ticks )
{
	unsigned int diff = SDL_GetTicks() - ticks;
	unsigned short delay = static_cast< unsigned short > ( ( frameDuration  - diff  ) + 0.5 );

	if ( diff < fpsLimit )
	{
		SDL_Delay( delay );
	}
}
void GameManager::Update( double delta )
{
	if ( isAIControlled )
		AIMove();

	UpdateNetwork();

	if ( menuManager.GetGameState() != GameState::InGame )
	{
		UpdateLobbyState();
		UpdateGUI();
		return;
	}

	UpdateBalls( delta );
	UpdateBullets( delta );
	UpdateBonusBoxes( delta );
	renderer.Update( delta );

	if ( IsTimeForNewBoard() )
	{
		std::cout << "GameMAanager@" << __LINE__ << " Level is done..\n";
		GenerateBoard();
	}

	if ( localPlayerLives == 0 && remotePlayerLives == 0 )
	{
		menuManager.SetGameState( GameState::GameOver );
		return;
	}

	UpdateGUI();
}
void GameManager::UpdateLobbyState()
{
	if ( menuManager.GetGameState() != GameState::Lobby || !menuManager.HasLobbyStateChanged() )
		return;

	switch ( menuManager.GetLobbyState() )
	{
		case LobbyMenuItem::NewGame:
			StartNewGame();
			break;
		case LobbyMenuItem::Update:
			UpdateGameList();
			break;
		case LobbyMenuItem::Back:
			GoBackFromLobby();
			break;
		case LobbyMenuItem::GameList:
			JoinGame();
			break;
		case LobbyMenuItem::Unknown:
			std::cout << "GameManager.cpp@" << __LINE__ << " Unkown new game state\n";
			break;
	}
}
void GameManager::StartNewGame()
{
	std::cout << "New game\n";
	SendNewGameMessage();
	menuManager.SetGameState( GameState::InGame );
	boardLoader.SetIsServer( true );
	netManager.SetIsServer( true );
	netManager.Connect( ip, port );
	// This is a temporary fix, setting the port to this client to something else
	// The code should be changed so that it's not necesseary to change port.
	port += 100;
}
void GameManager::GoBackFromLobby()
{
	menuManager.SetGameState( GameState::InGame );
	boardLoader.SetIsServer( false );
	netManager.SetIsServer( false );
	netManager.Connect( ip, port );
	menuManager.GoToMenu();
}

void GameManager::JoinGame()
{
	if ( menuManager.IsAnItemSelected() )
	{
		GameInfo gameInfo = menuManager.GetSelectedGameInfo();
		gameID = gameInfo.GetGameID();

		std::cout << "GameManager.cpp@" << __LINE__
			<< " Selected game in list : "<< gameInfo.GetAsSrting()
			<< std::endl;

		menuManager.SetGameState( GameState::InGame );
		boardLoader.SetIsServer( false );
		netManager.SetIsServer( false );
		netManager.Connect( gameInfo.GetIP(), static_cast< uint16_t > ( gameInfo.GetPort()  ) );

		SendJoinGameMessage( gameInfo );
	}
	else
		std::cout << "GameManager.cpp@" << __LINE__ << " No item selected\n";
}
void GameManager::UpdateGameList()
{
	menuManager.ClearGameList();
	SendGetGameListMessage();
}
void GameManager::AIMove()
{
	auto highest = FindHighestBall();

	if ( !IsTimeForAIMove( highest ) )
		return;

	double deadCenter = ( highest->rect.x + highest->rect.w / 2 ) -
		( ( localPaddle->rect.w / 2.0) *
		  Math::GenRandomNumber( -1.0, 1.0 ) );

	localPaddle->rect.x = deadCenter;
	SendPaddlePosMessage();
}
bool GameManager::IsTimeForAIMove( std::shared_ptr< Ball > highest ) const
{
	if ( !isAIControlled )
		return false;

	if ( !localPaddle )
		return false;

	if ( ballList.size() == 0 )
		return false;

	if ( highest == nullptr )
		return false;

	if ( highest->GetOwner() == Player::Remote )
		return false;

	if ( ( highest->rect.y + highest->rect.h ) <  localPaddle->rect.y  )
		return false;

	return true;
}

void GameManager::SetAIControlled( bool isAIControlled_ )
{
	isAIControlled = isAIControlled_;
}
std::shared_ptr< Ball > GameManager::FindHighestBall()
{
	double yMax = 0;
	std::shared_ptr< Ball > highest = nullptr;;
	for ( auto p : ballList )
	{
		if ( p->GetOwner() == Player::Local )
		{
			if ( p->rect.y > yMax )
			{
				highest = p;
				yMax = p->rect.y;
			}
		}
	}
	return highest;
}
void GameManager::CheckBallTileIntersection( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > closestTile = FindClosestIntersectingTile( ball );
	RemoveClosestTile( ball, closestTile );
}
void GameManager::RemoveClosestTile( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile )
{
	if ( !tile )
		return;

	if ( !ball->TileCheck( tile->rect, tile->GetObjectID() , IsSuperBall( ball )) )
		return;

	if ( ball->GetOwner() == Player::Local ) SendBallDataMessage( ball );

	tile->Hit();

	SendTileHitMessage( tile->GetObjectID() );

	UpdateTileHit( ball, tile );
}
bool GameManager::IsSuperBall( std::shared_ptr< Ball > ball )
{
	if ( ball->GetOwner() == Player::Local )
		return localPlayerSuperBall;
	else
		return remotePlayerSuperBall;
}
void GameManager::UpdateTileHit( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile )
{
	bool isDestroyed = tile->IsDestroyed();
	IncrementPoints( tile->GetTileTypeAsIndex(), isDestroyed, ball->GetOwner() );
	if ( isDestroyed )
	{
		if (tile->GetTileType() == TileType::Explosive )
		{
			int count = HandleExplosions( tile, ball->GetOwner() );
			AddBonusBox( ball, tile->rect.x, tile->rect.y, count );
		}
		else
		{
			AddBonusBox( ball, tile->rect.x, tile->rect.y );
			auto itClosestTile = std::find( tileList.begin(), tileList.end(), tile );
			tileList.erase( itClosestTile );
			RemoveTile( tile );
		}
	}
}
std::shared_ptr< Tile > GameManager::FindClosestIntersectingTile( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > closestTile;
	double closest = std::numeric_limits< double >::max();
	double current = closest;

	for ( auto p : tileList)
	{
		if ( !ball->CheckTileSphereIntersection( p->rect, ball->rect, current ) )
		{
			current = std::numeric_limits< double >::max();
			continue;
		}

		if ( current < closest )
		{
			closest = current;
			closestTile = p;
		}
	}

	return closestTile;
}
int GameManager::HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner )
{
	std::vector< Rect > rectVec = GenereateExplosionRects( explodingTile );

	int countDestroyedTiles = 0;

	auto isDeadFunc = [=, &countDestroyedTiles ]( std::shared_ptr< Tile > curr )
	{
		if ( !RectHelpers::CheckTileIntersection( rectVec, curr->rect) )
			return false;

		IncrementPoints( curr->GetTileTypeAsIndex(), true, ballOwner );
		renderer.RemoveTile( curr );
		++countDestroyedTiles;

		return true;
	};

	auto newEnd = std::remove_if( tileList.begin(), tileList.end(), isDeadFunc );

	// Remove item returned by remove_if
	tileList.erase( newEnd, tileList.end( ) );

	return countDestroyedTiles;
}
std::vector< Rect > GameManager::GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const
{
	std::vector< std::shared_ptr< Tile > > explodeVec = FindAllExplosiveTilesExcept( explodingTile );

	// The remaning tiles' vects are added if the tile intersects the originating explosions.
	Rect explodeRect( explodingTile->rect );
	explodeRect.DoubleRectSizes();

	std::vector< Rect > explodedTileRects;
	explodedTileRects.push_back( explodeRect );

	bool newExplosion = true;
	while ( explodeVec.size() > 0 && newExplosion )
	{
		newExplosion = false;

		for ( auto p : explodeVec )
		{
			if ( RectHelpers::CheckTileIntersection( explodedTileRects, p->rect) )
			{
				Rect r = p->rect;
				r.DoubleRectSizes();
				explodedTileRects.push_back( r );

				p->Kill();
				newExplosion = true;
			}
		}

		// Remove tiles marked as killed
		auto newEnd = std::remove_if( explodeVec.begin(), explodeVec.end(), []( std::shared_ptr< Tile > curr ){ return !curr->IsAlive(); } );
		explodeVec.erase( newEnd, explodeVec.end( ) );
	}

	return explodedTileRects;
}
std::vector< std::shared_ptr< Tile > > GameManager::FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const
{
	std::vector< std::shared_ptr< Tile > > explodingTileVec;

	// A simple lambda to only copy explosive and thoose that are different from explodinTile
	auto copyExplosive = [ explodingTile ]( std::shared_ptr< Tile > tile )
	{
		return  ( tile->GetTileType() == TileType::Explosive ) &&
			( tile != explodingTile );
	};

	// Back inserter because std::copy/std::copy_if expects an iterator that can write to the vector. The ones obtained by std::begin() can't.
	std::copy_if(
			tileList.begin(),
			tileList.end(),
			std::back_inserter( explodingTileVec ),
			copyExplosive
			);

	return explodingTileVec;
}
void GameManager::UpdateBonusBoxes( double delta )
{
	for ( auto p  : bonusBoxList )
	{
		if ( p->GetOwner() == Player::Local )
		{
			if ( p->rect.CheckTileIntersection( localPaddle->rect ) )
			{
				SendBonusBoxPickupMessage( p );
				ApplyBonus( p );
			}
		}
		// Remote BonusBoxe Pickup are handled by messages
	}

	MoveBonusBoxes ( delta );
	RemoveDeadBonusBoxes();
}

void GameManager::MoveBonusBoxes ( double delta )
{
	auto func = [ = ] ( std::shared_ptr< BonusBox > curr )
	{
		Vector2f direction = curr->GetDirection();

		curr->rect.x += direction.x * delta * curr->GetSpeed();
		curr->rect.y += direction.y * delta * curr->GetSpeed();

		if ( curr->rect.x < 0.0 || ( curr->rect.x + curr->rect.w ) > windowSize.w )
			curr->FlipXDir();

		return curr;
	};

	std::transform( bonusBoxList.begin(), bonusBoxList.end(), bonusBoxList.begin() , func );
}
void GameManager::RemoveDeadBonusBoxes()
{
	auto isDeadFunc = [=]( std::shared_ptr< BonusBox > curr )
	{
		if ( curr->IsAlive() )
			return false;

		renderer.RemoveBonusBox( curr );
		return true;
	};

	auto newEnd = std::remove_if( bonusBoxList.begin(), bonusBoxList.end(), isDeadFunc );

	// Remove item returned by remove_if
	bonusBoxList.erase( newEnd, bonusBoxList.end( ) );
}
void GameManager::ApplyBonus( std::shared_ptr< BonusBox > &ptr )
{
	switch  ( ptr->GetBonusType() )
	{
		case BonusType::ExtraLife:
			if ( ptr->GetOwner() == Player::Local )
			{
				++localPlayerLives;
				renderer.RenderText( "Extra Life!", Player::Local, true );
			}
			else
				++remotePlayerLives;
			break;
		case BonusType::Death:
			{
			if ( ptr->GetOwner() == Player::Local )
			{
				renderer.RenderText( "Death!!", Player::Local, true );
			}

			int32_t countKilled = 0;
			for ( auto p : ballList )
			{
				if ( p->GetOwner() == ptr->GetOwner() )
				{
					++countKilled;
					p->Kill();
					renderer.RemoveText();
					std::cout << "\tBallRemoved\n";
				}
			}
			if ( countKilled != 0 )
				DeleteDeadBalls();
			else
				ReducePlayerLifes( ptr->GetOwner() );
			}
			break;
		case BonusType::SuperBall:
			if ( ptr->GetOwner() == Player::Local )
			{
				localPlayerSuperBall = true;
				renderer.RenderText( "Super Ball!", Player::Local, true );
			}
			else
				remotePlayerSuperBall = true;
			break;
		default:
			break;
	}

	ptr->Kill();
}
BonusType GameManager::GetRandomBonusType() const
{
	int rand = Math::GenRandomNumber( 1000 );
	if ( rand < 500 )
		return BonusType::SuperBall;
	if ( rand < 750 )
		return BonusType::ExtraLife;
	else
		return BonusType::Death;

}
void GameManager::UpdateGUI( )
{
	if ( menuManager.GetGameState() == GameState::InGame )
		RenderInGame();
	else if ( menuManager.GetGameState() == GameState::GameOver )
		RenderEndGame();

	renderer.Render( );
	RendererScores();
}
void GameManager::RendererScores()
{
	renderer.RenderPoints   ( localPlayerPoints, Player::Local );
	renderer.RenderLives    ( localPlayerLives, Player::Local );
	renderer.RenderBallCount( localPlayerActiveBalls, Player::Local );

	if ( menuManager.IsTwoPlayerMode() )
	{
		renderer.RenderLives    ( remotePlayerLives, Player::Remote );
		renderer.RenderPoints   ( remotePlayerPoints, Player::Remote );
		renderer.RenderBallCount( remotePlayerActiveBalls, Player::Remote );
	}
}
void GameManager::RenderInGame()
{
	if ( menuManager.IsTwoPlayerMode()
			&& !isResolutionScaleRecieved
			&& netManager.IsServer()
	   )
	{
		renderer.RenderText( "Waiting for other player...", Player::Local  );
		netManager.Update();
		return;
	}

	if ( localPlayerActiveBalls == 0 )
	{
		if ( localPlayerLives == 0 )
		{
			if ( menuManager.IsTwoPlayerMode() )
			{
				if ( localPlayerPoints < remotePlayerPoints )
					renderer.RenderText( "Oh no, you lost :\'(", Player::Local  );
				else
					renderer.RenderText( "No more lives!", Player::Local  );
			}
			else
				renderer.RenderText( "No more lives!", Player::Local  );
		}
		else
			renderer.RenderText( "Press enter to launch ball", Player::Local  );
	}
	//else if ( ballList.size() > 0 )
		//renderer.RemoveText();
}
void GameManager::RenderEndGame()
{
	if ( menuManager.IsTwoPlayerMode() )
	{
		if ( localPlayerPoints > remotePlayerPoints )
			renderer.RenderText( "Yay, you won :D", Player::Local  );
		else
			renderer.RenderText( "Oh no, you lost :\'(", Player::Local  );
	}
	else
	{
		renderer.RenderText( "Game Over!", Player::Local  );
	}
}
void GameManager::GameManager::SetFPSLimit( unsigned short limit )
{
	fpsLimit  = limit;
	if ( fpsLimit > 0.0f )
		frameDuration = 1000.0 / static_cast< double > ( fpsLimit );
	else
		frameDuration = 0.0;
}
void GameManager::GenerateBoard()
{
	ClearBoard();

	if ( !boardLoader.IsLastLevel() )
	{
		std::cout << "GameManager@" << __LINE__ << " Last level..\n";
		menuManager.SetGameState( GameState::GameOver );
		return;
	}

	Board b = boardLoader.GenerateBoard( windowSize );
	std::vector<TilePosition> vec = b.GetTiles();

	for ( const auto &tile : vec )
		AddTile( tile.xPos, tile.yPos, tile.type );

	if (  menuManager.IsTwoPlayerMode() && netManager.IsServer()  )
		SetScale( b.GetScale() );
	else if ( !menuManager.IsTwoPlayerMode() )
		SetScale( b.GetScale()  );
}
bool GameManager::IsLevelDone()
{
	return CountDestroyableTiles() == 0;
}
int32_t GameManager::CountDestroyableTiles()
{
	if ( menuManager.GetGameState() != GameState::InGame && menuManager.GetGameState() != GameState::Paused )
		return 0;

	auto IsTileDestroyable = []( const std::shared_ptr< Tile > &tile ){ return ( tile->GetTileType() != TileType::Unbreakable ); };
	return static_cast< int32_t > ( std::count_if( tileList.begin(), tileList.end(), IsTileDestroyable ) );
}
bool GameManager::IsTimeForNewBoard()
{
	return ( ( !menuManager.IsTwoPlayerMode() || netManager.IsServer() ) &&  IsLevelDone() );
}
void GameManager::ClearBoard()
{
	tileList.clear();
	ballList.clear();

	localPlayerActiveBalls = 0;
	remotePlayerActiveBalls = 0;

	renderer.ClearBoard();
}
void GameManager::IncrementPoints( size_t tileType, bool isDestroyed, Player ballOwner )
{
	if ( ballOwner == Player::Local )
	{
		localPlayerPoints += 10;

		if ( isDestroyed )
			localPlayerPoints += points[ tileType ];
	} else if ( ballOwner == Player::Remote )
	{
		remotePlayerPoints += 10;

		if ( isDestroyed )
			remotePlayerPoints += points[ tileType ];
	}
}
void GameManager::ReducePlayerLifes( Player player )
{
	if ( player == Player::Local )
	{
		if ( localPlayerLives == 0 )
			return;

		--localPlayerLives;

		if ( localPlayerLives == 0 )
			RemoveDeadBallsAndBoxes( Player::Local );

		localPlayerSuperBall = false;
	}
	else
	{
		if ( remotePlayerLives == 0 )
			return;

		--remotePlayerLives;

		if ( remotePlayerLives == 0 )
			RemoveDeadBallsAndBoxes( Player::Remote );

		remotePlayerSuperBall = false;
	}
}
void GameManager::RemoveDeadBallsAndBoxes( Player player )
{
	for ( auto p : ballList )
	{
		if ( p->GetOwner() == player )
			p->Kill();
	}
	for ( auto p : bonusBoxList )
	{
		if ( p->GetOwner() == player )
			p->Kill();
	}
}
void GameManager::CreateMenu()
{
	menuManager.AddMenuElememts( renderer );
	menuManager.AddPauseMenuElememts( renderer );
	menuManager.AddLobbyMenuElememts( renderer );
}
void GameManager::SetLocalPaddlePosition( int x, int y )
{
	if ( x != 0 && y != 0 )
	{
		localPaddle->rect.x = static_cast< double > ( x ) - ( localPaddle->rect.w / 2 );

		if ( ( localPaddle->rect.x + localPaddle->rect.w ) > windowSize.w )
			localPaddle->rect.x = static_cast< double > ( windowSize.w ) - localPaddle->rect.w;

		if ( localPaddle->rect.x  <= 0  )
			localPaddle->rect.x = 0;

		SendPaddlePosMessage();
	}
}
void GameManager::SetScale( double scale_ )
{
	std::cout << "GameManager.cpp@" << __LINE__ << " Scale : " << scale_ << std::endl;
	scale = scale_;
	ApplyScale();
	SendGameSettingsMessage();
}
void GameManager::ApplyScale( )
{
	for ( auto& p : tileList )
	{
		p->rect.x = ( p->rect.x * scale ) + ( ( windowSize.w - ( windowSize.w * scale ) ) * 0.5 );
		p->rect.y = ( p->rect.y * scale ) + ( ( windowSize.h - ( windowSize.h * scale ) ) * 0.5 );

		p->ResetScale();
		p->SetScale( scale );
	}

	for ( auto& p : ballList )
	{
		p->ResetScale();
		p->SetScale( scale );
	}
}
void GameManager::ResetScale( )
{
	double tempScale = 1.0 / scale;
	scale = 1.0;

	for ( auto& p : tileList )
	{
		p->rect.x = ( p->rect.x * tempScale ) + ( ( windowSize.w - ( windowSize.w * tempScale ) ) * 0.5 );
		p->rect.y = ( p->rect.y * tempScale ) + ( ( windowSize.h - ( windowSize.h * tempScale ) ) * 0.5 );

		p->SetScale( tempScale );
	}

	for ( auto& p : ballList )
	{
		p->SetScale( tempScale );
	}
}

