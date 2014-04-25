#include "GameManager.h"

#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Paddle.h"
#include "structs/game_objects/Bullet.h"
#include "structs/game_objects/BonusBox.h"

#include "structs/board/Board.h"
#include "structs/net/TCPMessage.h"

#include "math/Math.h"
#include "math/RectHelpers.h"
#include "math/VectorHelpers.h"

#include "enums/MessageType.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MessageTarget.h"
#include "enums/ConfigValueType.h"

#include <vector>
#include <sstream>
#include <algorithm>

#include "Logger.h"
#include "BoardLoader.h"
#include "ConfigLoader.h"

	GameManager::GameManager()
	:	renderer()
	,	timer()
	,	menuManager( gameConfig )
	,	messageSender( netManager )
	,	physicsManager( messageSender )

	,	runGame( true )

	,	gameID( 0 )
	,	localPaddle()

	,	isAIControlled( false )

	,	ballList()
	,	windowSize()

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

	RenderMainText();

	physicsManager.SetWindowSize( windowSize );

	gameConfig.LoadConfig();
	LoadConfig();

	InitMenu();
	InitRenderer();
	InitPaddles();
	InitJoystick();

	logger = Logger::Instance();
	logger->Init( localPlayerName_ );

	return true;
}
void GameManager::InitMenu()
{
	menuManager.SetGameState( GameState::MainMenu );
	CreateMenu();
}
void GameManager::InitRenderer()
{
	RendererScores();
	renderer.RenderPlayerCaption( localPlayerInfo.name, Player::Local );
	renderer.Render( );
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

	netManager.Init( false  );
}
void GameManager::LoadConfig()
{
	physicsManager.SetBulletSpeed( gameConfig.Get( ConfigValueType::BulletSpeed ) );
	physicsManager.SetBonusBoxSpeed( gameConfig.Get( ConfigValueType::BonusBoxSpeed ) );

	localPlayerInfo.ballSpeed = gameConfig.Get( ConfigValueType::BallSpeed );
	remotePlayerInfo.ballSpeed = gameConfig.Get( ConfigValueType::BallSpeed );

	localPlayerInfo.fastMode = gameConfig.GetFastMode();
	remotePlayerInfo.fastMode = gameConfig.GetFastMode();
}
void GameManager::CreateMenu()
{
	renderer.InitGameList();
	renderer.InitConfigList();
	menuManager.SetGameList( renderer.GetGameList() );

	CreateMainMenu();
	CreatePauseMenu();
	CreateLobbyMenu();

	menuManager.SetOptionsMenuItem( renderer.GetOptionsBackButton() );
	menuManager.SetConfigList( renderer.GetConfigList() );
}
void GameManager::CreatePauseMenu()
{
	renderer.AddPauseMenuButtons( "Resume", "MainMenu", "Quit" );

	menuManager.SetPauseMenuItem( PauseMenuItemType::Resume, renderer.GetPauseMenuItem( PauseMenuItemType::Resume ) );
	menuManager.SetPauseMenuItem( PauseMenuItemType::MainMenu, renderer.GetPauseMenuItem( PauseMenuItemType::MainMenu ) );
	menuManager.SetPauseMenuItem( PauseMenuItemType::Quit, renderer.GetPauseMenuItem( PauseMenuItemType::Quit ) );
}
void GameManager::CreateLobbyMenu()
{
	renderer.AddLobbyMenuButtons( "New Game", "Update", "Back" );

	menuManager.SetLobbyMenuItem( LobbyMenuItem::NewGame, renderer.GetLobbyMenuItem( LobbyMenuItem::NewGame ) );
	menuManager.SetLobbyMenuItem( LobbyMenuItem::Update, renderer.GetLobbyMenuItem( LobbyMenuItem::Update ) );
	menuManager.SetLobbyMenuItem( LobbyMenuItem::Back, renderer.GetLobbyMenuItem( LobbyMenuItem::Back ) );
}
void GameManager::CreateMainMenu()
{
	renderer.AddMainMenuButtons( "Single Player", "Multiplayer", "Options", "Quit" );

	menuManager.SetMainMenuItem( MainMenuItemType::SinglePlayer,  renderer.GetMainMenuItem( MainMenuItemType::SinglePlayer ) );
	menuManager.SetMainMenuItem( MainMenuItemType::Options     ,  renderer.GetMainMenuItem( MainMenuItemType::Options) );
	menuManager.SetMainMenuItem( MainMenuItemType::MultiPlayer ,  renderer.GetMainMenuItem( MainMenuItemType::MultiPlayer) );
	menuManager.SetMainMenuItem( MainMenuItemType::Quit        ,  renderer.GetMainMenuItem( MainMenuItemType::Quit ) );
}
void GameManager::Restart()
{
	logger->Log( __FILE__, __LINE__, "======================= RESET =======================");

	boardLoader.Reset();
	ClearBoard();

	DeleteAllBonusBoxes();
	DeleteAllBullets();

	localPlayerInfo.Reset();
	remotePlayerInfo.Reset();

	LoadConfig();

	renderer.SetIsTwoPlayerMode( menuManager.IsTwoPlayerMode() );
	renderer.ResetText();

	RenderMainText();
	RendererScores();
}
void GameManager::AddBall( )
{
	if ( localPlayerInfo.activeBalls == 0 )
		AddBall( Player::Local, 0 );
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

		renderer.RenderBallCount( localPlayerInfo.activeBalls, Player::Local );
	}
	else
	{
		++remotePlayerInfo.activeBalls;

		renderer.RenderBallCount( localPlayerInfo.activeBalls, Player::Local );
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
void GameManager::RemoveBall( const std::shared_ptr< Ball >  &ball )
{
	ReduceActiveBalls( ball->GetOwner(), ball->GetObjectID() );

	renderer.RemoveBall( ball );
	physicsManager.RemoveBall( ball );

	RenderMainText();
}
void GameManager::ReduceActiveBalls( const Player &player, uint32_t ballID )
{
	if ( player ==  Player::Local )
	{
		--localPlayerInfo.activeBalls;

		messageSender.SendBallKilledMessage( ballID );

		if ( localPlayerInfo.activeBalls == 0 )
			ReducePlayerLifes( Player::Local );

		renderer.RenderBallCount( localPlayerInfo.activeBalls, Player::Local );
	}
	else
	{
		--remotePlayerInfo.activeBalls;

		if ( remotePlayerInfo.activeBalls == 0 )
			ReducePlayerLifes( Player::Remote );

		renderer.RenderBallCount( remotePlayerInfo.activeBalls, Player::Remote );
	}
}
void GameManager::AddTile( const Vector2f &pos, TileType tileType, int32_t tileID  )
{
	const auto &tile = physicsManager.CreateTile( pos, tileType, tileID  );

	if ( netManager.IsServer() )
		messageSender.SendTileSpawnMessage( tile, windowSize.h );

	tileList.push_back( tile );
	renderer.AddTile( tile );
}
void GameManager::RemoveTile( const std::shared_ptr< Tile > &tile )
{
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

	const auto &bonusBox = physicsManager.CreateBonusBox( 0, owner, dir, pos );

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );

	messageSender.SendBonusBoxSpawnedMessage( bonusBox, windowSize.h );
}
bool GameManager::WasBonusBoxSpawned( int32_t tilesDestroyed ) const
{
	double randMax = gameConfig.Get( ConfigValueType::BonusBoxChance );
	if ( tilesDestroyed != 1 )
	{
		double probabilityOfBonus = 1.0 / ( tilesDestroyed * 1000 );
		randMax = static_cast< int > ( probabilityOfBonus * randMax );
	}

	int32_t rand = static_cast< int32_t > ( RandomHelper::GenRandomNumber( ( randMax > 0 ) ? randMax : 1 ) );
	return (  rand == 1 );
}
void GameManager::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
{
	renderer.RemoveBonusBox( bb );
	physicsManager.RemoveBonusBox( bb );
}
void GameManager::UpdateBalls( double delta )
{
	CheckBallSpeedFastMode( delta );

	for ( const auto &p : ballList )
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
	for ( const auto  &bullet : bulletList )
	{
		bullet->Update( delta );

		if ( bullet->GetOwner()  == Player::Remote )
			continue;

		CheckBulletTileIntersections( bullet );
	}

	DeleteDeadBullets();
	DeleteDeadTiles();
}
void GameManager::CheckBulletOutOfBounds( const std::shared_ptr< Bullet > &bullet )
{
	if ( bullet->GetOwner() != Player::Local || !bullet->IsOutOfBounds() )
		return;

	bullet->Kill();

	if ( !IsSuperBullet( bullet->GetOwner() ) )
		return;

	const auto &deadTiles = physicsManager.FindAllTilesOnBulletsPath( bullet );

	for ( const auto &tile :deadTiles )
		tile->Kill();
}
void GameManager::CheckBulletTileIntersections( const std::shared_ptr< Bullet > &bullet )
{
	const auto &lowestTile = physicsManager.CheckBulletTileIntersections( bullet );

	if ( lowestTile != nullptr )
		HandleBulletTileIntersection( bullet, lowestTile );
}
void GameManager::HandleBulletTileIntersection( const std::shared_ptr< Bullet > &bullet, const std::shared_ptr< Tile > &tile )
{
	Player owner = bullet->GetOwner();

	renderer.GenerateParticleEffect( tile );

	if ( !IsSuperBullet( owner ) )
	{
		tile->Hit();
		bullet->Kill();
	}
	else
		tile->Kill();

	bool alive = tile->IsAlive();

	int32_t count = 1;
	if ( tile->GetTileType() == TileType::Explosive )
		count = HandleExplosions( tile, owner );
	else
	{
		IncrementPoints( tile->GetTileType(), !alive, owner );
		messageSender.SendTileHitMessage( tile->GetObjectID() );
		renderer.UpdateTileHit( tile );
	}

	DeleteDeadTiles();

	if ( !alive && bullet->GetOwner() == Player::Local )
		AddBonusBox( bullet->GetOwner(), Vector2f( 0.0f, 1.0f ), tile->GetPosition(), count );
}
bool GameManager::IsSuperBullet( const Player owner ) const
{
	if ( owner == Player::Local && localPlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		return true;

	else if ( owner == Player::Remote && remotePlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		return true;

	return false;
}
bool GameManager::AnyFastModeActive() const
{
	return ( localPlayerInfo.IsBonusActive( BonusType::SuperBall ) || remotePlayerInfo.IsBonusActive( BonusType::SuperBall ) );
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
			HandleRecieveMessage( msg );
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
			HandleRecieveMessage( msg );
	}
}
void GameManager::HandleRecieveMessage( const TCPMessage &message )
{
	switch ( message.GetType() )
	{
		case MessageType::GameSettings:
			remoteResolutionScale = windowSize.w / message.GetSize().x;
			break;
		case MessageType::GameStateChanged:
			menuManager.SetGameState( message.GetGameState() );
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
			ApplyBonus( physicsManager.GetBonusBoxWithID( message.GetObjectID(), Player::Remote ));
			break;
		case MessageType::PlayerName:
			renderer.RenderPlayerCaption( message.GetPlayerName(), Player::Remote );
			break;
		case MessageType::NewGame:
			RecieveNewGameMessage( message );
			break;
		case MessageType::GameJoined:
			RecieveJoinGameMessage( message );
			break;
		case MessageType::EndGame:
			UpdateGameList();
			break;
		case MessageType::BulletFire:
			RecieveBulletFireMessage( message );
			break;
		case MessageType::BulletKilled:
			RecieveBulletKillMessage( message );
			break;
		case MessageType::TileSpawned:
			AddTile(  message.GetPos1(), message.GetTileType(), message.GetObjectID()  );
			break;
		case MessageType::LastTileSent:
			logger->Log( __FILE__, __LINE__, "================================================================================");
			physicsManager.UpdateScale();
			break;
		default:
			logger->Log( __FILE__, __LINE__, "UpdateNetwork unknown message received", message );
			std::cin.ignore();
			break;
	}
}
void GameManager::RecieveJoinGameMessage( const TCPMessage &message  )
{
	if ( !netManager.IsConnected() )
	{
		logger->Log( __FILE__, __LINE__, "The client has accept the connection", message.GetObjectID() );
		netManager.Update();
	}

	GenerateBoard();
	UpdateGameList();

	menuManager.SetGameState( GameState::InGame );
	messageSender.SendPlayerName( localPlayerInfo.name );
}
void GameManager::RecieveNewGameMessage( const TCPMessage &message )
{
	GameInfo info;
	info.Set( message.GetIPAdress(), message.GetPort(), message.GetPlayerName()  );
	info.SetGameID( message.GetObjectID() );
	renderer.AddGameToList( info );
}
void GameManager::RecieveBallSpawnMessage( const TCPMessage &message )
{
	std::shared_ptr< Ball > ball = AddBall( Player::Remote, message.GetObjectID() );

	ball->SetPosition( Math::Scale( message.GetPos1(),  remoteResolutionScale ) );
	ball->SetDirection( message.GetDir() );
	ball->SetRemoteScale( remoteResolutionScale );

	renderer.RenderBallCount( remotePlayerInfo.activeBalls, Player::Remote );
}
void GameManager::RecieveBallDataMessage( const TCPMessage &message )
{
	std::shared_ptr< Ball > ball = physicsManager.GetBallWithID( message.GetObjectID(), Player::Remote );

	ball->SetPosition( Math::Scale( message.GetPos1(),  remoteResolutionScale ) );
	ball->SetDirection( message.GetDir() );
}
void GameManager::RecieveBallKillMessage( const TCPMessage &message )
{
	physicsManager.RemoveBallWithID( message.GetObjectID(), Player::Remote );
	DeleteDeadBalls();
}
void GameManager::RecieveTileHitMessage( const TCPMessage &message )
{
	std::shared_ptr< Tile > tile = physicsManager.GetTileWithID( message.GetObjectID() );

	renderer.GenerateParticleEffect( tile );

	if ( message.GetTileKilled() || remotePlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		tile->Kill();
	else
	{
		tile->Hit();
		renderer.UpdateTileHit( tile );
	}

	IncrementPoints( tile->GetTileType(), !tile->IsAlive(), Player::Remote );

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
	const auto &bonusBox = physicsManager.CreateBonusBox( message.GetObjectID(),  Player::Remote, message.GetDir_YFlipped(), Vector2f() );
	bonusBox->SetBonusType( message.GetBonusType() );
	bonusBox->SetPosition( Math::Scale( message.GetPos1(),  remoteResolutionScale ) );

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
}
std::shared_ptr< Bullet >  GameManager::FireBullet( int32_t id, const Player &owner, Vector2f pos )
{
	const auto &bullet = physicsManager.CreateBullet( id, owner, pos );

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
	physicsManager.KillBulletWithID(  message.GetObjectID(), Player::Remote );

	DeleteDeadBullets();
}
void GameManager::PrintRecv( const TCPMessage &msg, int32_t line  )
{
	logger->Log( __FILE__, line, msg.Print() );
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

	if ( deadTile )
		UpdateBoard();
}
void GameManager::DeleteDeadBullets()
{
	auto newEnd = std::remove_if(
		bulletList.begin(),
		bulletList.end(),
	[=]( std::shared_ptr< Bullet > &bullet )
	{
		if ( !bullet || bullet->IsAlive() )
			return false;

		renderer.RemoveBullet( (bullet ));
		physicsManager.RemoveBullet( bullet );

		if ( bullet->GetOwner() == Player::Local )
			messageSender.SendBulletKilledMessage( bullet->GetObjectID() );

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
	[=]( std::shared_ptr< Bullet > &bullet )
	{
		renderer.RemoveBullet( bullet );
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
	if (  !localPlayerInfo.CanFireBullet() )
		return;

	const auto &bullet1 = FireBullet
	(
		0,
		Player::Local,
		localPaddle->GetPosition()
	);

		Vector2f pos2 (localPaddle->rect.x+ localPaddle->rect.w - bullet1->rect.w, localPaddle->rect.y );
	const auto &bullet2 = FireBullet
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

		CheckForGameStateChange();

		Update( timer.GetDelta( ) );
	}
}
void GameManager::CheckForGameStateChange( )
{
	if ( !menuManager.HasGameStateChanged() )
		return;

	if ( !CheckGameLobbyChange( ) )
		return;

	UpdateGameState();

	HandleStatusChange();
}
void GameManager::HandleStatusChange()
{
	if ( menuManager.GetGameState() == GameState::Quit )
	{
		runGame = false;
	} else if ( menuManager.WasGameStarted() )
	{
		Restart();
		GenerateBoard();
	}
	else if ( menuManager.WasGameQuited() )
	{
		logger->Log( __FILE__, __LINE__, "Game was quited" );
		messageSender.SendEndGameMessage( gameID, ip, port );
		netManager.Close();
	}
	else if ( menuManager.WasGameResumed()  )
	{
		renderer.StartFade();
	}
}
bool GameManager::CheckGameLobbyChange( )
{
	if ( menuManager.GetGameState() == GameState::Lobby )
	{
		if ( !netManager.IsConnectedToGameServer() )
		{
			menuManager.SetGameState( GameState::MainMenu );
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Connection Error", "Could not connect to main server", NULL );
			return false;
		}
	}
	return true;
}
void GameManager::UpdateGameState()
{
	messageSender.SendGameStateChangedMessage( menuManager.GetGameState() );
	renderer.SetGameState( menuManager.GetGameState() );

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
			AddBall( );
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
			menuManager.CheckItemMouseOver( buttonEvent.x, buttonEvent.y);
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
			logger->Log( __FILE__, __LINE__, "Could noet grab joystick" );
	}
}
void GameManager::HandleJoystickEvent( const SDL_JoyButtonEvent &event )
{
	if ( event.state == SDL_PRESSED )
		AddBall( );
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
			case SDLK_y:
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
			case SDLK_c:
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

void GameManager::IncreaseBallSpeedFastMode( const Player &player, double delta )
{
	if ( player == Player::Local )
	{
		if ( localPlayerInfo.ballSpeed < gameConfig.Get( ConfigValueType::BallSpeed_FM ) )
		{
			localPlayerInfo.ballSpeed += delta * 1000;
			UpdateBallSpeed();
		}
	}
	else
	{
		if ( remotePlayerInfo.ballSpeed < gameConfig.Get( ConfigValueType::BallSpeed_FM ) )
		{
			remotePlayerInfo.ballSpeed += delta * 0.0005;
			UpdateBallSpeed();
		}
	}
}
void GameManager::Update( double delta )
{
	UpdateJoystick( );
	UpdateNetwork();
	RenderMainText();

	renderer.Update( delta );
	if ( menuManager.GetGameState() != GameState::InGame )
	{
		UpdateLobbyState();

		renderer.Render( );
		return;
	}

	IsGameOVer();
	AIMove();
	UpdateGameObjects( delta );
	renderer.Render( );
}
void GameManager::UpdateGameObjects( double delta )
{
	UpdateBalls( delta );
	UpdateBullets( delta );
	UpdateBonusBoxes( delta );
}
void GameManager::UpdateBoard()
{
	if ( menuManager.GetGameState() == GameState::GameOver )
		return;

	if ( IsLevelDone() && ( !menuManager.IsTwoPlayerMode()  || netManager.IsServer() ))
		GenerateBoard();
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
			logger->Log( __FILE__, __LINE__, "Unkown new game state" );
			break;
	}
}
void GameManager::StartNewGame()
{
	logger->Log( __FILE__, __LINE__, "New game" );
	messageSender.SendNewGameMessage( ip, port, localPlayerInfo.name );
	menuManager.SetGameState( GameState::InGameWait );
	netManager.SetIsServer( true );
	netManager.Connect( ip, port );

	Restart();

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

	Restart();
	messageSender.SendJoinGameMessage( gameInfo.GetGameID() );
}
void GameManager::UpdateGameList()
{
	renderer.ClearGameList();
	messageSender.SendGetGameListMessage();
}
void GameManager::AIMove()
{
	if ( !isAIControlled )
		return;

	physicsManager.AIMove();

	messageSender.SendPaddlePosMessage( localPaddle->rect.x );
}
void GameManager::CheckBallTileIntersection( const std::shared_ptr< Ball > &ball )
{
	std::shared_ptr< Tile > closestTile = physicsManager.FindClosestIntersectingTile( ball );
	RemoveClosestTile( ball, closestTile );
}
void GameManager::RemoveClosestTile( const std::shared_ptr< Ball > &ball, const std::shared_ptr< Tile > &tile )
{
	if ( !tile || !ball->TileCheck( tile->rect, IsSuperBall( ball )) )
		return;

	if ( ball->GetOwner() == Player::Local )
		messageSender.SendBallDataMessage( ball, windowSize.h );

	renderer.GenerateParticleEffect( tile );

	if ( localPlayerInfo.IsBonusActive( BonusType::SuperBall ) )
		tile->Kill();
	else
		tile->Hit();

	UpdateTileHit( ball, tile );
}
void GameManager::UpdateTileHit( const std::shared_ptr< Ball > &ball, const std::shared_ptr< Tile > &tile )
{
	int32_t count = 1;
	if (tile->GetTileType() == TileType::Explosive )
		count = HandleExplosions( tile, ball->GetOwner() );
	else
	{
		messageSender.SendTileHitMessage( tile->GetObjectID() );
		renderer.UpdateTileHit( tile );
		IncrementPoints( tile->GetTileType(), !tile->IsAlive(), ball->GetOwner() );
	}

	AddBonusBox( ball, tile->rect.x, tile->rect.y, count );
}
int GameManager::HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner )
{
	std::vector< Rect > rectVec = physicsManager.GenereateExplosionRects( explodingTile );

	int countDestroyedTiles = 0;

	const auto &isDeadFunc = [=, &countDestroyedTiles ]( const std::shared_ptr< Tile > &curr )
	{
		if ( !RectHelpers::CheckTileIntersection( rectVec, curr->rect) )
			return;

		IncrementPoints( curr->GetTileType(), true, ballOwner );
		messageSender.SendTileHitMessage( curr->GetObjectID(), true );
		renderer.GenerateParticleEffect( curr );
		++countDestroyedTiles;
		curr->Kill();
	};

	// Loop throug all and apply isDeadFunc
	std::for_each( tileList.begin(), tileList.end(), isDeadFunc );

	return countDestroyedTiles;
}
void GameManager::UpdateBonusBoxes( double delta )
{
	for ( const auto &p  : bonusBoxList )
	{
		if ( p->GetOwner() == Player::Local &&  p->rect.CheckTileIntersection( localPaddle->rect ) )
		{
			messageSender.SendBonusBoxPickupMessage( p->GetObjectID() );
			ApplyBonus( p );
		}
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
void GameManager::ApplyBonus( const std::shared_ptr< BonusBox > &ptr )
{
	switch  ( ptr->GetBonusType() )
	{
		case BonusType::ExtraLife:
			ApplyBonus_Life( ptr->GetOwner() );
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
void GameManager::ApplyBonus_Life( const Player &player )
{
	if ( player == Player::Local )
	{
		++localPlayerInfo.lives;
		renderer.RenderText( "Extra Life!", Player::Local, true );
		renderer.RenderLives( localPlayerInfo.lives, Player::Local );
	}
	else
	{
		++remotePlayerInfo.lives;
		renderer.RenderLives( remotePlayerInfo.lives, Player::Remote );
			}
}
void GameManager::ApplyBonus_Death( const Player &player )
{
	if ( player == Player::Remote )
	{
		renderer.RenderLives( remotePlayerInfo.lives, Player::Remote );
		return;
	}

	renderer.RenderText( "Death!!", Player::Local, true );

	if ( physicsManager.KillAllTilesWithOwner( player ) )
		DeleteDeadBalls();

	renderer.RenderLives( localPlayerInfo.lives, Player::Local );
}
void GameManager::RenderMainText( )
{
	if ( menuManager.GetGameState() == GameState::InGameWait )
		renderer.RenderText( "InGame : Wait...", Player::Local  );
	else
	if ( menuManager.GetGameState() == GameState::InGame )
	{
		if ( localPlayerInfo.CanSpawnNewBall() )
			renderer.RenderText( "Press enter to launch ball", Player::Local  );
		if ( localPlayerInfo.lives == 0 )
			renderer.RenderText( "No more lives!", Player::Local  );
	}
	else
	if ( menuManager.GetGameState() == GameState::GameOver )
	{
		if ( menuManager.IsTwoPlayerMode() )
		{
			if ( localPlayerInfo.points < remotePlayerInfo.points )
				renderer.RenderText( "Oh no, you lost :\'(", Player::Local  );
			else
				renderer.RenderText( "Yay, you won!!", Player::Local  );
		}
		else
			renderer.RenderText( "Game Over!", Player::Local  );
	}
}
void GameManager::RendererScores()
{
	renderer.RenderLives    ( localPlayerInfo.lives, Player::Local );
	renderer.RenderPoints   ( localPlayerInfo.points, Player::Local );
	renderer.RenderBallCount( localPlayerInfo.activeBalls, Player::Local );

	if ( menuManager.IsTwoPlayerMode() )
	{
		renderer.RenderPoints   ( remotePlayerInfo.points, Player::Remote );
		renderer.RenderLives    ( remotePlayerInfo.lives, Player::Remote );
		renderer.RenderBallCount( remotePlayerInfo.activeBalls, Player::Remote );
	}
}
void GameManager::GenerateBoard()
{
	if ( !CanGenerateNewBoard() )
		return;

	Board b = boardLoader.GenerateBoard( windowSize );
	std::vector<TilePosition> vec = b.GetTiles();

	for ( const auto &tile : vec )
		AddTile( tile.tilePos, tile.type, -1 );

	messageSender.SendLastTileMessage();
	logger->Log( __FILE__, __LINE__, "Loading of Board is done" );

	physicsManager.UpdateScale();
	//renderer.SetScale( physicsManager.GetScale() );
}
void GameManager::ClearBoard()
{
	logger->Log( __FILE__, __LINE__, "==================== Clear Board ====================");
	ballList.clear();

	DeleteAllBullets();

	localPlayerInfo.activeBalls = 0;
	remotePlayerInfo.activeBalls = 0;

	physicsManager.Clear();
	renderer.ClearBoard();
	tileList.erase( tileList.begin(), tileList.end() );
}
void GameManager::IncrementPoints( const TileType &tileType, bool isDestroyed, const Player &ballOwner )
{
	double pointIncrease = gameConfig.Get( ConfigValueType::PointsHit );

	if ( isDestroyed )
		pointIncrease += gameConfig.GetTilePoints( tileType );

	if ( ballOwner == Player::Local )
	{
		localPlayerInfo.points += pointIncrease;
		renderer.RenderPoints   ( localPlayerInfo.points, ballOwner );
	}
	else
	{
		remotePlayerInfo.points += pointIncrease;
		renderer.RenderPoints( remotePlayerInfo.points, ballOwner );
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

		renderer.RenderLives( localPlayerInfo.lives, Player::Local );
	}
	else
	{
		remotePlayerInfo.ReduceLifes();

		if ( remotePlayerInfo.lives == 0 )
			physicsManager.KillBallsAndBonusBoxes( Player::Remote );

		renderer.RenderLives( remotePlayerInfo.lives, Player::Local );
	}
}
bool GameManager::IsLevelDone()
{
	if ( physicsManager.CountAllTiles() == 0 || ( !AnyFastModeActive() && physicsManager.CountDestroyableTiles() == 0 ) )
	{
		ClearBoard();
		return true;
	}
	return false;
}
void GameManager::IsGameOVer()
{
	if ( localPlayerInfo.lives == 0 && remotePlayerInfo.lives == 0 )
		menuManager.SetGameState( GameState::GameOver );
}
bool GameManager::CanGenerateNewBoard()
{
	if ( menuManager.IsTwoPlayerMode() && !netManager.IsServer() )
	{
		logger->Log( __FILE__, __LINE__, " Not server! " );
		return false;
	}

	if ( boardLoader.IsLastLevel() )
	{
		logger->Log( __FILE__, __LINE__, "================== No more levels ===================" );
		menuManager.SetGameState( GameState::GameOver );

		return false;
	}

	return true;
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
bool GameManager::IsSuperBall( const std::shared_ptr< Ball > &ball )
{
	if ( ball->GetOwner() == Player::Local )
		return localPlayerInfo.IsBonusActive( BonusType::SuperBall );
	else
		return remotePlayerInfo.IsBonusActive( BonusType::SuperBall );
}
void GameManager::SetAIControlled( bool isAIControlled_ )
{
	isAIControlled = isAIControlled_;
}
void GameManager::SetFPSLimit( unsigned short limit )
{
	fpsLimit  = limit;
	if ( fpsLimit > 0.0f )
		frameDuration = 1000.0 / static_cast< double > ( fpsLimit );
	else
		frameDuration = 0.0;
}
