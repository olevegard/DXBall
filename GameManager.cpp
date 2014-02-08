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
	,	messageSender( netManager )
	,	physicsManager( messageSender )

	,	runGame( true )
	,	isOpnonentDoneWithLevel( false )

	,	gameID( 0 )
	,	localPaddle()

	,	isAIControlled( false )

	,	ballList()
	,	windowSize()
	,	scale( 1.0 )

	,	remoteResolutionScale( 1.0 )

	,	fpsLimit( 60 )
	,	frameDuration( 1000.0 / 60.0 )

	,	stick( nullptr )
{
	windowSize.x = 0;
	windowSize.y = 0;
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
 	physicsManager.SetWindowSize( windowSize );

	InitPaddles();
	InitMenu();
	InitJoystick();

	logger.Init( "log.txt", localPlayerName_ );
	logger.Log( __FILE__, __LINE__, "Init" );

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
	remotePaddle = std::make_shared< Paddle > ();

	physicsManager.SetPaddles( localPaddle, remotePaddle );
	physicsManager.SetPaddleData( );

	renderer.SetLocalPaddle( localPaddle );
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
	physicsManager.SetBulletSpeed( gameConfig.GetBulletSpeed() );
	physicsManager.SetBonusBoxSpeed( gameConfig.GetBonusBoxSpeed() );

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

	DeleteAllBonusBoxes();
	DeleteAllBullets();

	localPlayerInfo.Reset();
	remotePlayerInfo.Reset();

	LoadConfig();

	renderer.SetIsTwoPlayerMode( menuManager.IsTwoPlayerMode() );
	renderer.ResetText();

	UpdateGUI();
}
std::shared_ptr<Ball> GameManager::AddBall( )
{
	return AddBall( Player::Local, 0 );
}
std::shared_ptr<Ball> GameManager::AddBall( Player owner, unsigned int ballID )
{
	if ( menuManager.GetGameState() != GameState::InGame || !CanPlayerFireBall( owner ) )
		return nullptr;

	IncreaseActiveBalls( owner );

	std::shared_ptr< Ball >  ball;

	if ( owner == Player::Local )
	{
		ball = physicsManager.CreateBall( owner, 0, GetBallSpeed( owner ) );
		messageSender.SendBallSpawnMessage( ball, windowSize.h );
	}
	else
	{
		ball = physicsManager.CreateBall( owner, ballID, GetBallSpeed( owner ) );
	}


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
	physicsManager.RemoveBall( ball );

	UpdateGUI();
}
void GameManager::ReduceActiveBalls( const Player &player, uint32_t ballID )
{
	if ( player ==  Player::Local )
	{
		--localPlayerInfo.activeBalls;

		messageSender.SendBallKilledMessage( ballID );

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
	auto tile = physicsManager.CreateTile( posX, posY, tileType );

	tileList.push_back( tile );
	renderer.AddTile( tile );
}
void GameManager::RemoveTile( std::shared_ptr< Tile > tile )
{
	if ( tile == nullptr )
		return;

	tile->Kill();

	renderer.RemoveTile( tile );
	physicsManager.RemoveTile( tile );
}
void GameManager::AddBonusBox( const std::shared_ptr< Ball > &triggerBall, double x, double y, int tilesDestroyed /* = 1 */ )
{
	AddBonusBox( triggerBall->GetOwner(), triggerBall->GetDirection(), { x, y }, tilesDestroyed );
}
void GameManager::AddBonusBox( const Player &owner, Vector2f dir,  const Vector2f &pos, int tilesDestroyed )
{
	if (!WasBonusBoxSpawned( tilesDestroyed ) )
		return;

	auto bonusBox = physicsManager.CreateBonusBox( 0, owner, dir, pos );

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );

	messageSender.SendBonusBoxSpawnedMessage( bonusBox, windowSize.h );
}
bool GameManager::WasBonusBoxSpawned( int32_t tilesDestroyed ) const
{
	int randMax = gameConfig.GetBonusBoxChance();
	if ( tilesDestroyed != 1 )
	{
		double probabilityOfNoBonus = std::pow( 0.99, tilesDestroyed * 2);

		randMax = static_cast< int > ( probabilityOfNoBonus * 100 );
	}

	return ( RandomHelper::GenRandomNumber( ( randMax > 0 ) ? randMax : 1 ) == 1 );
}
void GameManager::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
{
	renderer.RemoveBonusBox( bb );
	physicsManager.RemoveBonusBox( bb );
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
			messageSender.SendBallDataMessage( p, windowSize.h );
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
void GameManager::CheckBulletTileIntersections( std::shared_ptr< Bullet > bullet )
{
	std::shared_ptr< Tile > lowestTile = physicsManager.CheckBulletTileIntersections( bullet );

	if ( lowestTile != nullptr )
		HandleBulletTileIntersection( bullet, lowestTile );
}
void GameManager::HandleBulletTileIntersection( std::shared_ptr< Bullet > bullet, std::shared_ptr< Tile > tile )
{
	Player owner = bullet->GetOwner();
	if ( owner == Player::Remote )
		return;

	messageSender.SendTileHitMessage( tile->GetObjectID() );

	if ( !IsSuperBullet( owner ) )
	{
		if ( bullet->IsAlive() )
			messageSender.SendBulletKilledMessage( bullet->GetObjectID() );

		tile->Hit();
		bullet->Kill();

	} else
		tile->Kill();

	IncrementPoints( tile->GetTileType(), !tile->IsAlive(), owner );

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

	if ( !menuManager.IsTwoPlayerMode() || !netManager.IsConnected() || menuManager.GetGameState() == GameState::InGameWait )
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
		case MessageType::TileSpawned:
			RecieveTileSpawnMessage( message );
			break;
		case MessageType::LastTileSent:
			physicsManager.UpdateScale();
			break;
		default:
			std::cout << "GameManager@" << __LINE__ << " UpdateNetwork unknown message received " << message << std::endl;
			std::cin.ignore();
			break;
	}
}
void GameManager::RecieveJoinGameMessage( const TCPMessage &message  )
{
	if ( !netManager.IsConnected() )
	{
		std::cout << "GameManager@" << __LINE__ << " Recieved GameJoined"
			"\n\tThis means the client has connectied, acccpting connection..."  << message.GetObjectID() << std::endl;
		netManager.Update();
	}

	UpdateGameList();

	menuManager.SetGameState( GameState::InGame );
	messageSender.SendPlayerName( localPlayerInfo.name );
}
void GameManager::RecieveNewGameMessage( const TCPMessage &message )
{
	GameInfo info;
	info.Set( message.GetIPAdress(), message.GetPort() );
	info.SetGameID( message.GetObjectID() );
	menuManager.AddGameToList( renderer, info );
}
void GameManager::RecieveEndGameMessage( const TCPMessage &message )
{
	std::cout << "GameManager@" << __LINE__ << " Game ended : " << message.GetObjectID() << std::endl;
	UpdateGameList();
	menuManager.ClearGameList();
}
void GameManager::RecievePlayerNameMessage( const TCPMessage &message )
{
	renderer.RenderPlayerCaption( message.GetPlayerName(), Player::Remote );
}
void GameManager::RecieveGameSettingsMessage( const TCPMessage &message)
{
	remoteResolutionScale = windowSize.w / message.GetSize().x;

	if ( !netManager.IsServer() || !menuManager.IsTwoPlayerMode()  )
	{
		GenerateBoard();
	}
}
void GameManager::RecieveGameStateChangedMessage( const TCPMessage &message)
{
	menuManager.SetGameState( message.GetGameState() );
}
void GameManager::RecieveLevelDoneMessage( const TCPMessage &message )
{
	std::cout << "ID : " << message.GetObjectID() << std::endl;
	GenerateBoard();
	isOpnonentDoneWithLevel = true;
}
void GameManager::RecieveBallSpawnMessage( const TCPMessage &message )
{
	std::shared_ptr< Ball > ball = AddBall( Player::Remote, message.GetObjectID() );

	ball->rect.x = message.GetPos1().x * remoteResolutionScale;
	ball->rect.y = message.GetPos1().y * remoteResolutionScale;

	ball->SetDirection( message.GetDir() );
	ball->SetRemoteScale( remoteResolutionScale );
}
void GameManager::RecieveTileSpawnMessage( const TCPMessage &message )
{
	AddTile(  message.GetPos1(), message.GetTileType()  );
}
void GameManager::RecieveBallDataMessage( const TCPMessage &message )
{
	if ( ballList.size() == 0 )
		return;

	std::shared_ptr< Ball > ball = physicsManager.GetBallWithID( message.GetObjectID(), Player::Remote );

	if ( !ball )
	{
		std::cout << "GameManager@" << __LINE__ << " Could not find ball with ID : " << message.GetObjectID() << std::endl;
		return;
	}

	// Need to add ball's height, because ball it traveling in oposite direction.
	// The board is also flipped, so the ball will have the oposite horizontal collision edge.
	ball->rect.x = message.GetPos1().x * remoteResolutionScale;
	ball->rect.y = ( message.GetPos1().y  * remoteResolutionScale ) - ball->rect.h ;

	ball->SetDirection( message.GetDir() );
}
void GameManager::RecieveBallKillMessage( const TCPMessage &message )
{
	physicsManager.RemoveBallWithID( message.GetObjectID(), Player::Remote );
	DeleteDeadBalls();
}
void GameManager::RecieveTileHitMessage( const TCPMessage &message )
{
	if ( tileList.size() == 0 )
		return;

	std::shared_ptr< Tile > tile = physicsManager.GetTileWithID( message.GetObjectID() );

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
	IncrementPoints( tile->GetTileType(), isDestroyed, Player::Remote );

	if ( tile->GetTileType() == TileType::Explosive )
	{
		HandleExplosions( tile, Player::Remote );
		return;
	}
	DeleteDeadTiles();
}
void GameManager::RecievePaddlePosMessage( const TCPMessage &message )
{
	if ( !remotePaddle )
		return;
	double xPos = message.GetPos1().x;
	if ( xPos > 0 && xPos < windowSize.w )
	{
		remotePaddle->rect.x = xPos;
	}
}
void GameManager::RecieveBonusBoxSpawnedMessage( const TCPMessage &message )
{
	auto bonusBox = physicsManager.CreateBonusBox( message.GetObjectID(),  Player::Remote, message.GetDir_YFlipped(), Vector2f() );
	bonusBox->SetBonusType( message.GetBonusType() );
	bonusBox->rect.x = message.GetPos1().x * remoteResolutionScale;
	bonusBox->rect.y = message.GetPos1().y * remoteResolutionScale - bonusBox->rect.h;

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
}
void GameManager::RecieveBonusBoxPickupMessage( const TCPMessage &message )
{
	auto bb = physicsManager.GetBonusBoxWithID( message.GetObjectID(), Player::Remote );

	if ( bb != nullptr )
		ApplyBonus( bb );
}
std::shared_ptr< Bullet >  GameManager::FireBullet( int32_t id, const Player &owner, Vector2f pos )
{
	auto bullet = physicsManager.CreateBullet( id, owner, pos );

	bulletList.push_back( bullet );
	renderer.AddBullet( bullet );

	return bullet;
}
void GameManager::RecieveBulletFireMessage( const TCPMessage &message )
{
	FireBullet( message.GetObjectID() , Player::Remote, message.GetPos1() );
	FireBullet( message.GetObjectID2(), Player::Remote, message.GetPos2() );
}
void GameManager::RecieveBulletKillMessage( const TCPMessage &message )
{
	if ( ballList.size() > 0 )
		physicsManager.KillBulletWithID( message.GetObjectID(), Player::Remote  );

	DeleteDeadBullets();
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
	physicsManager.UpdateBallSpeed( localPlayerInfo.ballSpeed, remotePlayerInfo.ballSpeed );
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

	if ( deadTile && tileList.size() == 0 && netManager.IsServer() )
		messageSender.SendLevelDoneMessage();
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
		physicsManager.RemoveBullet( bullet );

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
		physicsManager.RemoveBullet( bullet );

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
		physicsManager.RemoveBonusBox( bonusBox );

		return true;
	} );

	// Remove item returned by remove_if
	bonusBoxList.erase( newEnd, bonusBoxList.end( ) );
}
void GameManager::FireBullets()
{
	auto bullet1 = FireBullet
	(
		0,
		Player::Local,
		localPaddle->GetPosition()
	);

		Vector2f pos2 (localPaddle->rect.x+ localPaddle->rect.w - bullet1->rect.w, localPaddle->rect.y );
	auto bullet2 = FireBullet
	(
	 	0,
		Player::Local,
		pos2
	);

	messageSender.SendBulletFireMessage( bullet1, bullet2, windowSize.h );
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

		//DoFPSDelay( ticks );
	}
}
void GameManager::HandleStatusChange( )
{
	if ( !menuManager.HasGameStateChanged() )
		return;

	messageSender.SendGameStateChangedMessage( menuManager.GetGameState() );
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
		messageSender.SendEndGameMessage( gameID, ip, port );
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
		physicsManager.SetLocalPaddlePosition( buttonEvent.x );

		if ( buttonEvent.type == SDL_MOUSEBUTTONDOWN )
		{
			if ( localPlayerInfo.activeBalls == 0 )
				AddBall( );

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
}
void GameManager::HandleJoystickEvent( const SDL_JoyButtonEvent &event )
{
	if ( event.state == SDL_PRESSED )
		AddBall( );

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
				AddBall( );
				break;
			case SDLK_s:
				localPlayerInfo.SetBonusActive( BonusType::SuperBall, true );
				break;
			case SDLK_f:
				localPlayerInfo.SetBonusActive( BonusType::FireBullets, true );
				break;
			case SDLK_u:
				scale = physicsManager.ResetScale( );
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
			localPlayerInfo.ballSpeed += delta * 1000;
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
	if ( IsTimeForNewBoard() && ( !menuManager.IsTwoPlayerMode()  || netManager.IsServer() ))
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
	messageSender.SendNewGameMessage( ip, port );
	menuManager.SetGameState( GameState::InGameWait );
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

	netManager.SetIsServer( false );
	netManager.Connect( gameInfo.GetIP(), static_cast< uint16_t > ( gameInfo.GetPort()  ) );

	messageSender.SendJoinGameMessage( gameInfo.GetGameID() );
}
void GameManager::UpdateGameList()
{
	menuManager.ClearGameList();
	messageSender.SendGetGameListMessage();
}
void GameManager::AIMove()
{
	if ( !isAIControlled )
		return;

	auto highest = physicsManager.FindHighestBall();

	if ( !IsTimeForAIMove( highest ) )
		return;

	double deadCenter = ( highest->rect.x + highest->rect.w / 2 ) -
		( ( localPaddle->rect.w / 2.0) *
		  RandomHelper::GenRandomNumber( -1.0, 1.0 ) );

	localPaddle->rect.x = deadCenter;
	messageSender.SendPaddlePosMessage( localPaddle->rect.x );
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
void GameManager::CheckBallTileIntersection( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > closestTile = physicsManager.FindClosestIntersectingTile( ball );
	RemoveClosestTile( ball, closestTile );
}
void GameManager::RemoveClosestTile( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile )
{
	if ( !tile || !ball->TileCheck( tile->rect, IsSuperBall( ball )) )
		return;

	if ( ball->GetOwner() == Player::Local )
		messageSender.SendBallDataMessage( ball, windowSize.h );

	if ( localPlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		tile->Kill();
	else
		tile->Hit();

	messageSender.SendTileHitMessage( tile->GetObjectID() );

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
	IncrementPoints( tile->GetTileType(), isDestroyed, ball->GetOwner() );

	if ( !isDestroyed )
		return;

	int32_t count = 1;
	if (tile->GetTileType() == TileType::Explosive )
		count = HandleExplosions( tile, ball->GetOwner() );

	AddBonusBox( ball, tile->rect.x, tile->rect.y, count );
}
int GameManager::HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner )
{
	std::vector< Rect > rectVec = physicsManager.GenereateExplosionRects( explodingTile );

	int countDestroyedTiles = 0;

	auto isDeadFunc = [=, &countDestroyedTiles ]( std::shared_ptr< Tile > curr )
	{
		if ( !RectHelpers::CheckTileIntersection( rectVec, curr->rect) )
			return;

		IncrementPoints( curr->GetTileType(), true, ballOwner );
		++countDestroyedTiles;
		curr->Kill();
	};

	// Loop throug all and apply isDeadFunc
	std::for_each( tileList.begin(), tileList.end(), isDeadFunc );

	return countDestroyedTiles;
}
void GameManager::UpdateBonusBoxes( double delta )
{
	for ( auto p  : bonusBoxList )
	{
		if ( p->GetOwner() == Player::Local )
		{
			if ( p->rect.CheckTileIntersection( localPaddle->rect ) )
			{
				messageSender.SendBonusBoxPickupMessage( p->GetObjectID() );
				ApplyBonus( p );
			}
		}
		// Remote BonusBoxe Pickup are handled by messages
	}

	physicsManager.MoveBonusBoxes ( delta );
	RemoveDeadBonusBoxes();
}
void GameManager::RemoveDeadBonusBoxes()
{
	auto isDeadFunc = [=]( std::shared_ptr< BonusBox > curr )
	{
		if ( curr->IsAlive() )
			return false;

		renderer.RemoveBonusBox( curr );
		physicsManager.RemoveBonusBox( curr );

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

	if ( physicsManager.KillAllTilesWithOwner( player ) )
		DeleteDeadBalls();
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
void GameManager::SetFPSLimit( unsigned short limit )
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

	for ( const auto &tile : vec )
		AddTile( tile.xPos, tile.yPos, tile.type );

	std::cout << "GameManager@" << __LINE__ << " Generate board | Scale : " << b.GetScale() << "\n";

	SetScale( b.GetScale() );
}
bool GameManager::IsLevelDone()
{
	return physicsManager.CountDestroyableTiles() == 0;
}
bool GameManager::IsTimeForNewBoard()
{
	return ( tileList.size() == 0 || ( menuManager.IsTwoPlayerMode() &&  isOpnonentDoneWithLevel ) );
}
void GameManager::ClearBoard()
{
	tileList.clear();
	ballList.clear();

	DeleteAllBullets();
	DeleteAllBonusBoxes();

	localPlayerInfo.activeBalls = 0;
	remotePlayerInfo.activeBalls = 0;

	physicsManager.Clear();
	renderer.ClearBoard();
}
void GameManager::IncrementPoints( const TileType &tileType, bool isDestroyed, Player ballOwner )
{
	if ( ballOwner == Player::Local )
	{
		localPlayerInfo.points += gameConfig.GetPointsHit();

		if ( isDestroyed )
			localPlayerInfo.points += gameConfig.GetTilePoints( tileType );
	}
	else if ( ballOwner == Player::Remote )
	{
		remotePlayerInfo.points += gameConfig.GetPointsHit();

		if ( isDestroyed )
			remotePlayerInfo.points += gameConfig.GetTilePoints( tileType );
	}
}
void GameManager::ReducePlayerLifes( Player player )
{
	LoadConfig();

	if ( player == Player::Local )
	{
		localPlayerInfo.ReduceLifes();

		if ( localPlayerInfo.lives == 0 )
			physicsManager.KillBallsAndBonusBoxes( Player::Local );
	}
	else
	{
		remotePlayerInfo.ReduceLifes();

		if ( remotePlayerInfo.lives == 0 )
			physicsManager.KillBallsAndBonusBoxes( Player::Remote );
	}
}
void GameManager::SetScale( double scale_ )
{
	std::cout << "GameManager@" << __LINE__ << " Scale : " << scale_ << std::endl;
	scale = scale_;
	physicsManager.ApplyScale( scale );
}
void GameManager::SetAIControlled( bool isAIControlled_ )
{
	isAIControlled = isAIControlled_;
}
