#include "GameManager.h"

#include "structs/board/Board.h"
#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Paddle.h"
#include "structs/game_objects/Bullet.h"
#include "structs/net/TCPMessage.h"

#include "math/Math.h"
#include "math/RectHelpers.h"

#include "enums/MessageType.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MessageTarget.h"

#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>

#include "BoardLoader.h"
#include "ConfigLoader.h"


	GameManager::GameManager()
	:	renderer()
	,	timer()

	,	runGame( true )
	,	isOpnonentDoneWithLevel( false )

	,	gameID( 0 )
	,	localPaddle()

	,	isAIControlled( false )

	,	ballList()
	,	windowSize()
	,	scale( 1.0 )
	,	points{ 20, 50, 100, 500 }

	,	remoteResolutionScale( 1.0 )

	,	tileCount( 0 )
	,	ballCount( 0 )
	,	bonusCount( 0 )
	,	bulletCount( 0 )

	,	fpsLimit( 60 )
	,	frameDuration( 1000.0 / 60.0 )

	,	stick( nullptr )
{
	windowSize.x = 0.0;
	windowSize.y = 0.0;
	windowSize.w = 1920 / 2;
	windowSize.h = 1080 / 2;
}

bool GameManager::Init( const std::string &localPlayerName_,  const SDL_Rect &size, bool startFS )
{
	localPlayerInfo.name = localPlayerName_;

	windowSize = size;
	bool server = localPlayerName_ ==  "server";

	if ( !renderer.Init( windowSize, startFS, server ) )
		return false;

	UpdateGUI();

	renderer.RenderPlayerCaption( localPlayerInfo.name, Player::Local );

	InitPaddles();
	InitMenu();
	InitJoystick();

	LoadConfig();

	return true;
}
void GameManager::InitMenu()
{
	menuManager.Init( renderer );
	menuManager.SetGameState( GameState::MainMenu );
	CreateMenu();
}
void GameManager::InitPaddles()
{
	localPaddle = std::make_shared< Paddle > ();
	localPaddle->textureType = TextureType::e_Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h - ( localPaddle->rect.h * 1.5 );
	localPaddle->SetScale( scale );
	std::cout << "GameManager@" << __LINE__ << " Local paddle : " << localPaddle << std::endl;
	renderer.SetLocalPaddle( localPaddle );

	remotePaddle = std::make_shared< Paddle > ();
	remotePaddle->textureType = TextureType::e_Paddle;
	remotePaddle->rect.w = 120;
	remotePaddle->rect.h = 30;
	remotePaddle->rect.x = 400;
	remotePaddle->rect.y = remotePaddle->rect.h * 0.5;
	std::cout << "GameManager@" << __LINE__ << " Remote paddle : " << remotePaddle << std::endl;
	renderer.SetRemotePaddle( remotePaddle );
}
void GameManager::InitNetManager( std::string ip_, uint16_t port_ )
{
	ip = ip_;
	port = port_;
	std::cout << "GameManager@" << __LINE__ <<  " IP : " << ip << " | Port : " << port << "\n";
	netManager.Init( false  );

	GameInfo gameInfo;
	gameInfo.Set( ip, port );
}
void GameManager::LoadConfig()
{
	gameConfig.LoadConfig();

	localPlayerInfo.ballSpeed = gameConfig.GetBallSpeed();
	remotePlayerInfo.ballSpeed = gameConfig.GetBallSpeed();
}
void GameManager::CreateMenu()
{
	menuManager.AddMenuElememts( renderer );
	menuManager.AddPauseMenuElememts( renderer );
	menuManager.AddLobbyMenuElememts( renderer );
}
void GameManager::Restart()
{
	std::cout
		<< "=============================="
		<< " RESTART "
		<< "=============================="
		<< std::endl;

	boardLoader.Reset();

	if ( !menuManager.IsTwoPlayerMode() )
	{
		boardLoader.SetIsServer( true );
	}
	else
	{
		remotePaddle->SetScale( scale );
	}

	DeleteAllBonusBoxes();
	DeleteAllBullets();

	GenerateBoard();

	localPlayerInfo.Reset();
	remotePlayerInfo.Reset();

	LoadConfig();

	renderer.SetIsTwoPlayerMode( menuManager.IsTwoPlayerMode() );
	renderer.ResetText();

	UpdateGUI();
}
std::shared_ptr<Ball> GameManager::AddBall( Player owner, unsigned int ballID )
{
	if ( menuManager.GetGameState() != GameState::InGame || !CanPlayerFireBall( owner ) )
		return nullptr;

	IncreaseActiveBalls( owner );

	auto ball = LaunchBall( owner, ballID );

	if ( owner == Player::Local )
		SendBallSpawnMessage( ball );

	++ballCount;

	return ball;
}
std::shared_ptr< Ball >  GameManager::LaunchBall( const Player &player, uint32_t ballID )
{
	std::shared_ptr< Ball > ball = std::make_shared< Ball >( windowSize, player, ballID );
	ball->textureType = TextureType::e_Ball;
	ball->SetScale( scale );
	ball->SetSpeed( GetBallSpeed( player ) );

	ballList.push_back( ball );
	renderer.AddBall( ball );

	return ball;
}
void GameManager::IncreaseActiveBalls( const Player &player )
{
	if ( player == Player::Local )
	{
		if ( localPlayerInfo.activeBalls == 0 )
			renderer.StartFade();

		++localPlayerInfo.activeBalls;
	}
	else
	{
		++remotePlayerInfo.activeBalls;
	}
}
double GameManager::GetBallSpeed( const Player &player ) const
{
	if ( player == Player::Local )
		return localPlayerInfo.ballSpeed;
	else
		return remotePlayerInfo.ballSpeed;
}
bool GameManager::CanPlayerFireBall( const Player &player ) const
{
	if (  player == Player::Local && localPlayerInfo.lives == 0 )
		return false;
	else if ( player == Player::Remote && remotePlayerInfo.lives == 0)
		return false;

	return true;
}
void GameManager::RemoveBall( const std::shared_ptr< Ball >  ball )
{
	if ( !ball )
		return;

	ReduceActiveBalls( ball->GetOwner(), ball->GetObjectID() );

	renderer.RemoveBall( ball );
	UpdateGUI();
}
void GameManager::ReduceActiveBalls( const Player &player, uint32_t ballID )
{
	if ( player ==  Player::Local )
	{
		--localPlayerInfo.activeBalls;

		SendBallKilledMessage( ballID );

		if ( localPlayerInfo.activeBalls == 0 )
			ReducePlayerLifes( Player::Local );
	}
	else
	{
		--remotePlayerInfo.activeBalls;

		if ( remotePlayerInfo.activeBalls == 0 )
			ReducePlayerLifes( Player::Remote );
	}
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

	tile->Kill();

	renderer.RemoveTile( tile );

	// Decrement tile count
	--tileCount;
}
void GameManager::AddBonusBox( const std::shared_ptr< Ball > &triggerBall, double x, double y, int tilesDestroyed /* = 1 */ )
{
	AddBonusBox( triggerBall->GetOwner(), triggerBall->GetDirection(), { x, y }, tilesDestroyed );
}
void GameManager::AddBonusBox( const Player &owner, Vector2f dir,  const Vector2f &pos, int tilesDestroyed )
{
	if (!WasBonusBoxSpawned( tilesDestroyed ) )
		return;

	std::shared_ptr< BonusBox > bonusBox  = std::make_shared< BonusBox > ( bonusCount++ );

	SetBonusBoxData( bonusBox, owner, pos );
	SetBonusBoxDirection( bonusBox, dir );

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
	SendBonusBoxSpawnedMessage( bonusBox );
}
bool GameManager::WasBonusBoxSpawned( int32_t tilesDestroyed ) const
{
	int randMax = gameConfig.GetBonusBoxChance();
	if ( tilesDestroyed != 1 )
	{
		double probabilityOfNoBonus = std::pow( 0.99, tilesDestroyed * 2);

		randMax = static_cast< int > ( probabilityOfNoBonus * 100 );
	}

	return ( Math::GenRandomNumber( ( randMax > 0 ) ? randMax : 1 ) == 1 );
}
void GameManager::SetBonusBoxData( std::shared_ptr< BonusBox > bonusBox, const Player &owner, const Vector2f &pos  ) const
{
	bonusBox->rect.x = pos.x;
	bonusBox->rect.y = pos.y;

	bonusBox->SetOwner( owner  );
	bonusBox->SetBonusType( GetRandomBonusType()  );
	bonusBox->SetSpeed( gameConfig.GetBonusBoxSpeed());
}
void GameManager::SetBonusBoxDirection( std::shared_ptr< BonusBox > bonusBox, Vector2f dir_ ) const
{
	Vector2f dir = dir_;
	if ( bonusBox->GetOwner()  == Player::Local )
		dir.y = ( dir.y > 0.0 ) ? dir.y : dir.y * -1.0;
	else
		dir.y = ( dir.y < 0.0 ) ? dir.y : dir.y * -1.0;

	bonusBox->SetDirection( dir );
}
void GameManager::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
{
	renderer.RemoveBonusBox( bb );
}
void GameManager::UpdateBalls( double delta )
{
	CheckBallSpeedFastMode( delta );

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
	for ( auto  bullet : bulletList )
	{
		bullet->Update( delta );

		CheckBulletTileIntersections( bullet );
	}

	if ( !gameConfig.IsFastMode() || !localPlayerInfo.bonusMap[BonusType::SuperBall] )
		DeleteDeadBullets();
	DeleteDeadTiles();
}
bool GameManager::DidBulletHitTile( std::shared_ptr< Bullet > bullet, std::shared_ptr< Tile > tile )
{
	if ( !tile->IsAlive() )
		return false;

	if ( !bullet->HasHitTile( tile->rect ) )
		return false;

	if ( localPlayerInfo.IsBonusActive( BonusType::SuperBall ) )
	{
		HandleBulletTileIntersection( bullet, tile );
		return false;
	}

	return true;
}
void GameManager::CheckBulletTileIntersections( std::shared_ptr< Bullet > bullet )
{
	bool bulletHitTile = false;
	std::shared_ptr< Tile > lowestTile;
	double lowestTileY = 0;

	for ( auto tile : tileList )
	{
		if ( !DidBulletHitTile( bullet, tile ) )
			continue;

		bulletHitTile = true;

		if ( tile->rect.y > lowestTileY )
		{
			lowestTile = tile;
			lowestTileY = tile->rect.y;
		}
	}
	if ( bulletHitTile )
		HandleBulletTileIntersection( bullet, lowestTile );
}
void GameManager::HandleBulletTileIntersection( std::shared_ptr< Bullet > bullet, std::shared_ptr< Tile > tile )
{
	Player owner = bullet->GetOwner();
	if ( owner == Player::Remote )
		return;

	SendTileHitMessage( tile->GetObjectID() );

	if ( !IsSuperBullet( owner ) )
	{
		if ( bullet->IsAlive() )
			SendBulletKilledMessage( bullet );
		tile->Hit();
		bullet->Kill();

	} else
		tile->Kill();

	IncrementPoints( tile->GetTileTypeAsIndex(), !tile->IsAlive(), owner );

	if ( tile->IsAlive() )
		return;

	int32_t count = 1;
	if ( tile->GetTileType() == TileType::Explosive )
		count= HandleExplosions( tile, owner );

	if ( bullet->GetOwner() == Player::Local )
		AddBonusBox( bullet->GetOwner(), Vector2f( 0.0f, 1.0f ),  Vector2f( tile->rect.x, tile->rect.y ), count );
}
bool GameManager::IsSuperBullet( const Player owner ) const
{
	if ( !gameConfig.IsFastMode() )
		return false;

	if ( owner == Player::Local && localPlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		return true;

	else if ( owner == Player::Remote && remotePlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		return true;

	return false;
}
void GameManager::UpdateNetwork()
{
	ReadMessagesFromServer();

	netManager.Update();

	if ( !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() )
		return;

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
		case MessageType::BulletFire:
			RecieveBulletFireMessage( message );
			break;
		case MessageType::BulletKilled:
			RecieveBulletKillMessage( message );
			break;
		case MessageType::LevelDone:
			RecieveLevelDoneMessage( message );
			break;
		default:
			std::cout << "GameManager@" << __LINE__ << " UpdateNetwork unknown message received " << message << std::endl;
			std::cin.ignore();
			break;
	}
}

void GameManager::RecieveJoinGameMessage( const TCPMessage &message  )
{
	UpdateGameList();

	SendGameSettingsMessage();
	SendPlayerName();

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
	}
}
void GameManager::RecieveGameStateChangedMessage( const TCPMessage &message)
{
	menuManager.SetGameState( message.GetGameState() );
	PrintRecv( message );
}
void GameManager::RecieveLevelDoneMessage( const TCPMessage &message )
{
	isOpnonentDoneWithLevel = true;
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
	PrintRecv( message );
	if ( ballList.size() > 0 )
	{
		auto ball =  GetBallFromID( message.GetObjectID() );

		if ( ball == nullptr )
		{
			std::cout << "GameManager@" << __LINE__ << " Ball with ID : " << message.GetObjectID() << " doesn't exist\n";
			return;
		}

		ball->Kill();
	}

	DeleteDeadBalls();
}
void GameManager::RecieveTileHitMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	if ( tileList.size() == 0 )
		return;

	std::shared_ptr< Tile > tile = GetTileFromID( message.GetObjectID() );

	if ( tile == nullptr )
	{
		std::cout << "GameManager@" << __LINE__ << " Tile with ID : " << message.GetObjectID() << " doesn't exist\n";
		return;
	}
	if ( remotePlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		tile->Kill();
	else
		tile->Hit();

	bool isDestroyed = !tile->IsAlive();
	IncrementPoints( tile->GetTileTypeAsIndex(), isDestroyed, Player::Remote );

	if ( tile->GetTileType() == TileType::Explosive )
	{
		HandleExplosions( tile, Player::Remote );
		return;
	}
	DeleteDeadTiles();
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
	bonusBox->SetSpeed( gameConfig.GetBonusBoxSpeed());

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
}
void GameManager::RecieveBonusBoxPickupMessage( const TCPMessage &message )
{
	PrintRecv( message );
	auto bb = GetBonusBoxFromID( message.GetObjectID() );

	if ( bb == nullptr )
	{
		std::cout << "GameManager@" << __LINE__ << " BonusBox with ID : " << message.GetObjectID() << " doesn't exist\n";
		return;
	}

	ApplyBonus( bb );
}
void GameManager::RecieveBulletFireMessage( const TCPMessage &message )
{
	std::shared_ptr< Bullet > bullet = std::make_shared< Bullet >( message.GetObjectID() );
	bullet->SetPosition( message.GetXPos(),  message.GetYPos() );
	bullet->SetSpeed( gameConfig.GetBulletSpeed() );
	bullet->SetOwner( Player::Remote );
	bulletList.push_back( bullet );
	renderer.AddBullet( bullet );

	std::shared_ptr< Bullet > bullet2 = std::make_shared< Bullet >( message.GetObjectID2() );
	bullet2->SetPosition( message.GetXPos2(), message.GetYPos2() );
	bullet2->SetSpeed( gameConfig.GetBulletSpeed() );
	bullet2->SetOwner( Player::Remote );
	bulletList.push_back( bullet2 );
	renderer.AddBullet( bullet2 );
}
void GameManager::RecieveBulletKillMessage( const TCPMessage &message )
{
	if ( ballList.size() > 0 )
	{
		auto p = GetBulletFromID( message.GetObjectID() );

		if ( p == nullptr )
		{
			std::cout << "GameManager@" << __LINE__ << " BonusBox with ID : " << message.GetObjectID() << " doesn't exist\n";
			return;
		}

		p->Kill();
	}

	DeleteDeadBullets();
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

	menuManager.SetGameState( GameState::InGame );
	PrintSend(msg);
}
void GameManager::SendPlayerName()
{
	TCPMessage msg;

	msg.SetMessageType( MessageType::PlayerName );
	msg.SetPlayerName( localPlayerInfo.name );

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
void GameManager::SendBallKilledMessage( uint32_t ballID )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::BallKilled );
	msg.SetObjectID(  ballID  );

	SendMessage( msg, MessageTarget::Oponent );

	//PrintSend(msg);
}
void GameManager::SendTileHitMessage( unsigned int tileID )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	auto p = GetTileFromID( tileID );
	if ( p == nullptr )
	{
		std::cout << "GameManager@" << __LINE__ << " Tile with ID : " << tileID << " doesn't exist\n";
		return;
	}

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
void GameManager::SendBulletFireMessage( const std::shared_ptr< Bullet > &bulletLeft, const std::shared_ptr< Bullet > &bulletRight  )
{
	TCPMessage msg;
	std::stringstream ss;

	msg.SetMessageType( MessageType::BulletFire );
	msg.SetObjectID( bulletLeft->GetObjectID() );
	msg.SetObjectID2( bulletRight->GetObjectID() );

	msg.SetXPos( bulletLeft->rect.x );
	msg.SetYPos( windowSize.h - bulletLeft->rect.y );

	msg.SetXPos2( bulletRight->rect.x );
	msg.SetYPos2( windowSize.h - bulletRight->rect.y );

	ss << msg;
	netManager.SendMessage( ss.str() );

}
void GameManager::SendBulletKilledMessage( const std::shared_ptr< Bullet > &bullet )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::BulletKilled );
	msg.SetObjectID(  bullet->GetObjectID() );

	SendMessage( msg, MessageTarget::Oponent );

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
void GameManager::SendLevelDoneMessage( )
{
	if ( !menuManager.IsTwoPlayerMode() )
		return;

	TCPMessage msg;

	msg.SetMessageType( MessageType::LevelDone );
	msg.SetObjectID( 0 );

	SendMessage( msg, MessageTarget::Oponent );
	PrintSend( msg );
}
void GameManager::SendNewGameMessage( )
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::NewGame );
	msg.SetIPAdress( ip  );
	msg.SetPort( port );

	SendMessage( msg, MessageTarget::Server );

	PrintSend( msg );
}
void GameManager::SendJoinGameMessage( const GameInfo &gameInfo )
{
	TCPMessage msg;
	msg.SetMessageType( MessageType::GameJoined );
	msg.SetObjectID( gameInfo.GetGameID() );

	SendMessage( msg, MessageTarget::Server );

	PrintSend( msg );
}
void GameManager::SendEndGameMessage( )
{
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

	//PrintSend( msg );
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
void GameManager::UpdateBallSpeed()
{
	auto setBallSpeed = [=]( std::shared_ptr< Ball > curr )
	{
		if ( curr->GetOwner() == Player::Local )
			curr->SetSpeed( localPlayerInfo.ballSpeed );
		else

			curr->SetSpeed( remotePlayerInfo.ballSpeed );
	};
	std::for_each( ballList.begin(), ballList.end(), setBallSpeed );
}
void GameManager::DeleteDeadTiles()
{
	bool deadTile = false;
	auto newEnd = std::remove_if(
		tileList.begin(),
		tileList.end(),
	[=, &deadTile]( std::shared_ptr< Tile > tile )
	{
		if ( !tile || tile->IsAlive() )
			return false;

		deadTile = true;

		RemoveTile( tile );

		return true;
	} );

	// Remove item returned by remove_if
	tileList.erase( newEnd, tileList.end( ) );

	if ( deadTile && tileList.size() == 0 )
		SendLevelDoneMessage();
}
void GameManager::DeleteDeadBullets()
{
	auto newEnd = std::remove_if(
		bulletList.begin(),
		bulletList.end(),
	[=]( std::shared_ptr< Bullet > bullet )
	{
		if ( !bullet || bullet->IsAlive() )
			return false;

		renderer.RemoveBullet( (bullet ));
		//bulletList.erase( p );

		return true;
	} );

	// Remove item returned by remove_if
	bulletList.erase( newEnd, bulletList.end( ) );

}
void GameManager::DeleteAllBullets()
{
	auto newEnd = std::remove_if(
		bulletList.begin(),
		bulletList.end(),
	[=]( std::shared_ptr< Bullet > bullet )
	{
		renderer.RemoveBullet( (bullet ));

		return true;
	} );

	// Remove item returned by remove_if
	bulletList.erase( newEnd, bulletList.end( ) );
}
void GameManager::DeleteAllBonusBoxes()
{
	auto newEnd = std::remove_if(
		bonusBoxList.begin(),
		bonusBoxList.end(),
	[=]( std::shared_ptr< BonusBox > bonusBox )
	{
		renderer.RemoveBonusBox( bonusBox );

		return true;
	} );

	// Remove item returned by remove_if
	bonusBoxList.erase( newEnd, bonusBoxList.end( ) );
}
void GameManager::FireBullets()
{
	std::shared_ptr< Bullet > bullet = std::make_shared< Bullet >( bulletCount++ );
	bullet->SetOwner( Player::Local );
	bullet->SetPosition( localPaddle->rect.x, localPaddle->rect.y - 10 );
	bullet->SetSpeed( gameConfig.GetBulletSpeed() );
	bulletList.push_back( bullet );
	renderer.AddBullet( bullet );

	std::shared_ptr< Bullet > bullet2 = std::make_shared< Bullet >( bulletCount++ );
	bullet2->SetOwner( Player::Local );
	bullet2->SetPosition( localPaddle->rect.x + localPaddle->rect.w - bullet2->rect.w, localPaddle->rect.y - 10 );
	bullet2->SetSpeed( gameConfig.GetBulletSpeed() );
	bulletList.push_back( bullet2 );
	renderer.AddBullet( bullet2 );

	SendBulletFireMessage( bullet, bullet2 );
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
		std::cout << "GameMaanager@" << __LINE__ << " GameState::Lobby..\n";
		UpdateGameList();
	}
	else if ( menuManager.WasGameStarted()  )
	{
		Restart();
	}
	else if ( menuManager.WasGameQuited() )
	{
		std::cout << "GameMAanager@" << __LINE__
			<< " Game was quited game state : " << static_cast< int32_t > ( menuManager.GetGameState() )
			<< " prev : " << static_cast< int32_t > ( menuManager.GetPrevGameState() )
			<< std::endl;
		SendEndGameMessage();
		netManager.Close();
	}
	else if ( menuManager.GetGameState() == GameState::InGame && menuManager.GetPrevGameState() == GameState::Paused )
	{
		renderer.StartFade();
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
	else if ( event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP )
	{
		HandleJoystickEvent( event.jbutton );
	}
}
void GameManager::HandleMouseEvent(  const SDL_MouseButtonEvent &buttonEvent )
{
	if ( menuManager.GetGameState() == GameState::InGame && !isAIControlled )
	{
		SetLocalPaddlePosition( buttonEvent.x, buttonEvent.y );

		if ( buttonEvent.type == SDL_MOUSEBUTTONDOWN )
		{
			if ( localPlayerInfo.activeBalls == 0 )
				AddBall( Player::Local, ballCount );

			if ( localPlayerInfo.IsBonusActive( BonusType::FireBullets ) )
				FireBullets();
		}
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
void GameManager::InitJoystick()
{
	if ( SDL_NumJoysticks() > 0 )
	{
		stick = SDL_JoystickOpen( 0 );
		if ( stick == nullptr )
			std::cout << "GameMaanager@" << __LINE__ << " Could not grab joystick\n";
	}
	else
		std::cout << "GameMaanager@" << __LINE__ << " No  joysticks where found!\n";

}
void GameManager::HandleJoystickEvent( const SDL_JoyButtonEvent &event )
{
	if ( event.state == SDL_PRESSED )
		AddBall( Player::Local, ballCount );

	//std::cout << "eve " << static_cast< int32_t > ( event.button ) << std::endl;
}
void GameManager::UpdateJoystick( )
{
	DirectionX dirX = GetJoystickDirection( 0 );
	DoJoystickMovement( dirX );
}
DirectionX GameManager::GetJoystickDirection( int32_t axis )
{
	DirectionX dirX = DirectionX::Middle;
	if ( SDL_JoystickGetAxis( stick, axis ) > 0 )
		dirX = DirectionX::Right;
	else if ( SDL_JoystickGetAxis( stick, axis ) < 0 )
		dirX = DirectionX::Left;

	return dirX;
}
void GameManager::DoJoystickMovement( const DirectionX &dirX )
{
	if ( dirX == DirectionX::Right )
	{
		localPaddle->rect.x += 10;
		if ( localPaddle->rect.x > windowSize.w )
			localPaddle->rect.x = 0;
	}
	else if ( dirX == DirectionX::Left )
	{
		localPaddle->rect.x -= 10;
		if ( localPaddle->rect.x < 0 )
			localPaddle->rect.x = windowSize.w;
	}
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
			case SDLK_n:
				StartNewGame();
				break;
			case SDLK_u:
				UpdateGameList();
				break;
			default:
				break;
		}
	}

}
void GameManager::HandleGameKeys( const SDL_Event &event )
{
	if ( menuManager.GetGameState() != GameState::InGame )
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
				AddBall( Player::Local, ballCount );
				break;
			case SDLK_s:
				localPlayerInfo.SetBonusActive( BonusType::SuperBall, true );
				break;
			case SDLK_f:
				localPlayerInfo.SetBonusActive( BonusType::FireBullets, true );
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
void GameManager::CheckBallSpeedFastMode( double delta )
{
	if ( !gameConfig.IsFastMode() )
		return;

	if ( localPlayerInfo.IsBonusActive( BonusType::SuperBall ) && localPlayerInfo.IsBonusActive( BonusType::FireBullets ) )
		IncreaseBallSpeedFastMode( Player::Local, delta );

	if ( remotePlayerInfo.IsBonusActive( BonusType::SuperBall ) && remotePlayerInfo.IsBonusActive( BonusType::FireBullets ) )
		IncreaseBallSpeedFastMode( Player::Remote, delta );
}
void GameManager::IncreaseBallSpeedFastMode( const Player &player, double delta )
{
	if ( player == Player::Local )
	{
		if ( localPlayerInfo.ballSpeed < gameConfig.GetBallSpeedFastMode())
		{
			localPlayerInfo.ballSpeed += delta * 0.0005;
			UpdateBallSpeed();
		}
	}
	else
	{
		if ( remotePlayerInfo.ballSpeed < gameConfig.GetBallSpeedFastMode() )
		{
			remotePlayerInfo.ballSpeed += delta * 0.0005;
			UpdateBallSpeed();
		}
	}
}
void GameManager::Update( double delta )
{
	UpdateJoystick( );
	UpdateGUI();
	UpdateNetwork();

	if ( menuManager.GetGameState() != GameState::InGame )
	{
		UpdateLobbyState();
		return;
	}

	AIMove();
	UpdateBoard();
	UpdateGameObjects( delta );
	renderer.Update( delta );
	CheckIfGameIsOver();
}
void GameManager::UpdateGameObjects( double delta )
{
	UpdateBalls( delta );
	UpdateBullets( delta );
	UpdateBonusBoxes( delta );
}
void GameManager::UpdateBoard()
{
	if ( IsTimeForNewBoard() )
	{
		std::cout << "GameManager@" << __LINE__ << " Generating board...\n";
		DeleteAllBullets();
		GenerateBoard();
	}
}
void GameManager::CheckIfGameIsOver()
{
	if ( localPlayerInfo.lives == 0 && remotePlayerInfo.lives == 0 )
	{
		std::cout << "GameManager@" << __LINE__
			<< " =========="
			<< " GAME OVER "
			<< " =========="
			<< std::endl;
		menuManager.SetGameState( GameState::GameOver );
	}
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
			menuManager.GoToMenu();
			break;
		case LobbyMenuItem::GameList:
			JoinGame();
			break;
		case LobbyMenuItem::Unknown:
			std::cout << "GameManager@" << __LINE__ << " Unkown new game state\n";
			break;
	}
}
void GameManager::StartNewGame()
{
	std::cout << "GameManager@" << __LINE__ << " New game\n";
	SendNewGameMessage();
	menuManager.SetGameState( GameState::InGameWait );
	boardLoader.SetIsServer( true );
	netManager.SetIsServer( true );
	netManager.Connect( ip, port );

	// This is a temporary fix, setting the port to this client to something else
	// The code should be changed so that it's not necesseary to change port.
	port += 100;
}
void GameManager::JoinGame()
{
	if ( !menuManager.IsAnItemSelected() )
		return;

	GameInfo gameInfo = menuManager.GetSelectedGameInfo();
	gameID = gameInfo.GetGameID();

	std::cout << "GameManager@" << __LINE__
		<< " Selected game in list : "<< gameInfo.GetAsSrting()
		<< std::endl;

	boardLoader.SetIsServer( false );
	netManager.SetIsServer( false );
	netManager.Connect( gameInfo.GetIP(), static_cast< uint16_t > ( gameInfo.GetPort()  ) );

	SendJoinGameMessage( gameInfo );
}
void GameManager::UpdateGameList()
{
	menuManager.ClearGameList();
	SendGetGameListMessage();
}
void GameManager::AIMove()
{
	if ( !isAIControlled )
		return;

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
	if ( !tile || !ball->TileCheck( tile->rect, IsSuperBall( ball )) )
		return;

	if ( ball->GetOwner() == Player::Local )
		SendBallDataMessage( ball );

	if ( localPlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		tile->Kill();
	else
		tile->Hit();

	SendTileHitMessage( tile->GetObjectID() );

	UpdateTileHit( ball, tile );
}
bool GameManager::IsSuperBall( std::shared_ptr< Ball > ball )
{
	if ( ball->GetOwner() == Player::Local )
		return localPlayerInfo.IsBonusActive( BonusType::SuperBall );
	else
		return remotePlayerInfo.IsBonusActive( BonusType::SuperBall );
}
void GameManager::UpdateTileHit( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile )
{
	bool isDestroyed = !tile->IsAlive();
	IncrementPoints( tile->GetTileTypeAsIndex(), isDestroyed, ball->GetOwner() );

	if ( !isDestroyed )
		return;

	int32_t count = 1;
	if (tile->GetTileType() == TileType::Explosive )
		count = HandleExplosions( tile, ball->GetOwner() );

	AddBonusBox( ball, tile->rect.x, tile->rect.y, count );
}
std::shared_ptr< Tile > GameManager::FindClosestIntersectingTile( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > closestTile;
	double closest = std::numeric_limits< double >::max();
	double current = closest;

	for ( auto p : tileList)
	{
		if ( !p->IsAlive() )
			continue;

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
			return;

		IncrementPoints( curr->GetTileTypeAsIndex(), true, ballOwner );
		++countDestroyedTiles;
		curr->Kill();
	};

	// Loop throug all and apply isDeadFunc
	std::for_each( tileList.begin(), tileList.end(), isDeadFunc );

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
				++localPlayerInfo.lives;
				renderer.RenderText( "Extra Life!", Player::Local, true );
			}
			else
				++remotePlayerInfo.lives;
			break;
		case BonusType::Death:
			ApplyBonus_Death( ptr->GetOwner() );
			break;
		case BonusType::SuperBall:
			if ( ptr->GetOwner() == Player::Local )
			{
				localPlayerInfo.SetBonusActive( BonusType::SuperBall, true);
				renderer.RenderText( "Super Ball!", Player::Local, true );
			}
			else
				remotePlayerInfo.SetBonusActive( BonusType::SuperBall, true);
			break;
		case BonusType::FireBullets:
			if ( ptr->GetOwner() == Player::Local )
			{
				localPlayerInfo.SetBonusActive( BonusType::FireBullets, true);
				renderer.RenderText( "Lazarz!", Player::Local, true );
			}
			else
				remotePlayerInfo.SetBonusActive( BonusType::FireBullets, true);
			break;

		default:

			break;
	}

	ptr->Kill();
}
void GameManager::ApplyBonus_Death( const Player &player )
{
	if ( player == Player::Local )
		renderer.RenderText( "Death!!", Player::Local, true );

	if ( KillAllTilesWithOwner( player ) )
		// Delete all dead Balls, will in turns reduce Player life
		DeleteDeadBalls();
}
bool GameManager::KillAllTilesWithOwner( const Player &player )
{
	bool tilesKilled = false;
	for ( auto p : ballList )
	{
		if ( p->GetOwner() == player )
		{
			p->Kill();
			tilesKilled = true;
		}
	}
	return tilesKilled;
}
BonusType GameManager::GetRandomBonusType() const
{
	int rand = Math::GenRandomNumber( 1000 );
	if ( rand < 250 )
		return BonusType::FireBullets;
	if ( rand < 500 )
		return BonusType::SuperBall;
	if ( rand < 750 )
		return BonusType::ExtraLife;
	else
		return BonusType::Death;

}
void GameManager::UpdateGUI( )
{
	if ( menuManager.GetGameState() == GameState::InGame || menuManager.GetGameState() == GameState::InGameWait )
		RenderInGame();
	else if ( menuManager.GetGameState() == GameState::GameOver )
		RenderEndGame();

	renderer.Render( );
	RendererScores();
}
void GameManager::RendererScores()
{
	renderer.RenderPoints   ( localPlayerInfo.points, Player::Local );
	renderer.RenderLives    ( localPlayerInfo.lives, Player::Local );
	renderer.RenderBallCount( localPlayerInfo.activeBalls, Player::Local );

	if ( menuManager.IsTwoPlayerMode() )
	{

		renderer.RenderPoints   ( remotePlayerInfo.points, Player::Remote );
		renderer.RenderLives    ( remotePlayerInfo.lives, Player::Remote );
		renderer.RenderBallCount( remotePlayerInfo.activeBalls, Player::Remote );
	}
}
void GameManager::RenderInGame()
{
	if ( menuManager.GetGameState() == GameState::InGameWait )
	{
		renderer.RenderText( "InGame : Wait...", Player::Local  );
		return;
	}

	if ( localPlayerInfo.activeBalls != 0 )
		return;

	if ( localPlayerInfo.lives != 0 )
	{
		renderer.RenderText( "Press enter to launch ball", Player::Local  );
		return;
	}

	RenderEndGame();
}
void GameManager::RenderEndGame()
{
	if ( !menuManager.IsTwoPlayerMode() )
	{

		if ( localPlayerInfo.lives == 0 )
			renderer.RenderText( "No more lives!", Player::Local  );
		else
			renderer.RenderText( "No more levels!", Player::Local  );
		return;
	}

	if ( !IsLevelDone() && remotePlayerInfo.lives > 0 )
	{
		renderer.RenderText( "Waiting for oponent...", Player::Local );
		return;
	}

	if ( localPlayerInfo.points < remotePlayerInfo.points )
		renderer.RenderText( "Oh no, you lost :\'(", Player::Local  );
	else
		renderer.RenderText( "Yay, you won!!", Player::Local  );
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
	isOpnonentDoneWithLevel = false;

	if ( !boardLoader.IsLastLevel() )
	{
		std::cout << "GameManager@" << __LINE__
			<< " =========="
			<< " GAME OVER "
			<< " =========="
			<< std::endl;

		menuManager.SetGameState( GameState::GameOver );
		return;
	}

	Board b = boardLoader.GenerateBoard( windowSize );
	std::vector<TilePosition> vec = b.GetTiles();

	if ( vec.size() == 0 )
		SendLevelDoneMessage();

	for ( const auto &tile : vec )
		AddTile( tile.xPos, tile.yPos, tile.type );

	std::cout << "GameManager@" << __LINE__ << " Generate board | Scale : " << b.GetScale() << "\n";

	SetScale( b.GetScale() );
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
	return ( tileList.size() == 0 && ( !menuManager.IsTwoPlayerMode() ||  isOpnonentDoneWithLevel ) );
}
void GameManager::ClearBoard()
{
	tileList.clear();
	tileCount = 0;

	ballList.clear();
	ballCount = 0;

	DeleteAllBullets();
	DeleteAllBonusBoxes();

	localPlayerInfo.activeBalls = 0;
	remotePlayerInfo.activeBalls = 0;

	renderer.ClearBoard();
}
void GameManager::IncrementPoints( size_t tileType, bool isDestroyed, Player ballOwner )
{
	if ( ballOwner == Player::Local )
	{
		localPlayerInfo.points += 10;

		if ( isDestroyed )
			localPlayerInfo.points += points[ tileType ];
	} else if ( ballOwner == Player::Remote )
	{

		remotePlayerInfo.points += 10;

		if ( isDestroyed )
			remotePlayerInfo.points += points[ tileType ];
	}
}
void GameManager::ReducePlayerLifes( Player player )
{
	LoadConfig();

	if ( player == Player::Local )
	{
		localPlayerInfo.ReduceLifes();

		if ( localPlayerInfo.lives == 0 )
			KillBallsAndBonusBoxes( Player::Local );
	}
	else
	{
		remotePlayerInfo.ReduceLifes();

		if ( remotePlayerInfo.lives == 0 )
			KillBallsAndBonusBoxes( Player::Remote );
	}
}
void GameManager::KillBallsAndBonusBoxes( const Player &player )
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
	std::cout << "GameManager@" << __LINE__ << " Scale : " << scale_ << std::endl;
	scale = scale_;
	ApplyScale();
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
std::shared_ptr< Bullet > GameManager::GetBulletFromID( int32_t ID )
{
	for ( auto p : bulletList )
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
