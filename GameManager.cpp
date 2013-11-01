#include "GameManager.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"
#include "BoardLoader.h"

#include "math/Math.h"
#include "math/RectHelpers.h"
#include "Board.h"

#include "TCPMessage.h"

#include "enums/MessageType.h"

#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <cmath>


	GameManager::GameManager()
	:	runGame( true )
	,	renderer()
	,	timer()
	,	localPaddle()

	,	isTwoPlayerMode( false )
	,	localPlayerPoints( 0 )
	,	localPlayerLives( 3 )
	,	localPlayerActiveBalls( 0 )

	,	remotePlayerPoints( 0 )
	,	remotePlayerLives( 3 )
	,	remotePlayerActiveBalls( 0 )

	,	ballList()
	,	windowSize()
	,	scale( 1.0 )
	,	points{ 20, 50, 100, 500 }

	,	remoteResolutionScale( 1.0 )
	,	isResolutionScaleRecieved( false )

	,	tileCount( 0 )
	,	ballCount( 0 )
	,	fpsLimit( 60 )
	,	frameDuration( 1000.0 / 60.0 )
{
	windowSize.x = 0.0;
	windowSize.y = 0.0;
	windowSize.w = 1920 / 2;
	windowSize.h = 1080 / 2;
}

bool GameManager::Init( const std::string &localPlayerName, const std::string &remotePlayerName, const SDL_Rect &size, bool startFS )
{
	windowSize = size;
	bool server = localPlayerName ==  "server";

	if ( !renderer.Init( windowSize, startFS, server ) )
		return false;

	UpdateGUI();

	renderer.RenderPlayerCaption( localPlayerName, Player::Local );
	renderer.RenderPlayerCaption( remotePlayerName, Player::Remote );

	CreateMenu();

	localPaddle = std::make_shared< Paddle > ();
	localPaddle->textureType = TextureType::e_Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h - ( localPaddle->rect.h * 1.5 );
	localPaddle->SetScale( scale );

	renderer.SetLocalPaddle( localPaddle );

	return true;
}

void GameManager::InitNetManager( bool isServer, std::string ip, unsigned short port )
{
	netManager.Init( isServer, ip, port );
	boardLoader.SetIsServer( !isTwoPlayerMode || isServer );
}
void GameManager::Restart()
{
	if ( isTwoPlayerMode )
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
	GenerateBoard();

	localPlayerPoints = 0;
	localPlayerLives = 3;
	localPlayerActiveBalls = 0;

	remotePlayerPoints = 0;
	remotePlayerLives = 3;
	remotePlayerActiveBalls = 0;

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


	if ( scale != 1.0 )
		ball->SetScale( scale );

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

		if ( localPlayerActiveBalls == 0 )
			--localPlayerLives;

		SendBallKilledMessage( ball );
	} else if ( ball->GetOwner() == Player::Remote )
	{
		--remotePlayerActiveBalls;

		if ( remotePlayerActiveBalls == 0 )
			--remotePlayerLives;
		renderer.RenderBallCount( remotePlayerActiveBalls, Player::Remote );
	}

	renderer.RemoveBall( ball );
	UpdateGUI();
}


void GameManager::AddTile( short posX, short posY, TileType tileType )
{
	std::shared_ptr< Tile > tile = std::make_shared< Tile >( tileType, tileCount++ );
	tile->textureType = TextureType::e_Tile;

	tile->rect.x = posX;//( posX * scale ) + ( ( windowSize.w - ( windowSize.w * scale ) ) * 0.5 );
	tile->rect.y = posY;//( posY * scale ) + ( ( windowSize.h - ( windowSize.h * scale ) ) * 0.5 );
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
	int randMax = 100;
	if ( tilesDestroyed != 1 )
	{
		double probabilityOfNoBonus = std::pow( 0.99, tilesDestroyed * 2);

		randMax = static_cast< int > ( probabilityOfNoBonus * 100 );
	}

	if ( Math::GenRandomNumber( ( randMax > 0 ) ? randMax : 1 ) != 1 )
		return;

	std::shared_ptr< BonusBox > bonusBox  = std::make_shared< BonusBox > ();
	bonusBox->rect.x = x;
	bonusBox->rect.y = y;

	Player ballOwner = triggerBall->GetOwner();

	Vector2f direction = triggerBall->GetDirection();

	if ( ballOwner == Player::Local )
		// Force y dir to be positive ( down )
		direction.y = ( direction.y > 0.0 ) ? direction.y : direction.y * -1.0;
	else
		direction.y = ( direction.y < 0.0 ) ? direction.y : direction.y * -1.0;

	bonusBox->SetDirection( direction );
	bonusBox->SetOwner( ballOwner  );

	bonusBoxList.push_back( bonusBox );
	renderer.AddBonusBox( bonusBox );
}
void GameManager::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
{
	renderer.RemoveBonusBox( bb );
}
void GameManager::UpdateBalls( double delta )
{
	if ( ballList.size() > 0 )
		renderer.RemoveText();

	for ( auto p : ballList )
	{
		p->Update( delta );

		if ( p->GetOwner() == Player::Remote )
			continue;

		if ( p->BoundCheck( windowSize ) )
			SendBallDataMessage( p );

		if ( p->PaddleCheck( localPaddle->rect ) )
			SendBallDataMessage( p );

		CheckBallTileIntersection( p );

		if ( p->DeathCheck( windowSize ) )
			p->Kill();
	}

	DeleteDeadBalls();
}
void GameManager::UpdateNetwork()
{
	if ( netManager.IsConnected()  && localPaddle )
	{
		TCPMessage msg;

		// Reading
		bool stop = false;
		while ( !stop )
		{
			std::string str = netManager.ReadMessage();

			if ( str == "" )
			{
				stop = true;
				break;
			}

			std::stringstream ss;
			ss << str;

			while ( ss >> msg )
			{
				HandleRecieveMessage( msg );
			}
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
		default:
			std::cout << "GameManager@" << __LINE__ << " : UpdateNetwork message received " << message << std::endl;
			std::cin.ignore();
			break;
	}
}

void GameManager::RecieveGameSettingsMessage( const TCPMessage &message)
{
	remoteResolutionScale = windowSize.w / message.GetXSize();
	PrintRecv( message );

	std::cout << "Remote board scale : " << message.GetBoardScale()
			  << " * " << remoteResolutionScale
			  << " = " << message.GetBoardScale() * remoteResolutionScale  << std::endl;

	if ( !netManager.IsServer() )
		SetScale( message.GetBoardScale() * remoteResolutionScale );

	isResolutionScaleRecieved = true;
}
void GameManager::RecieveBallSpawnMessage( const TCPMessage &message )
{
	PrintRecv( message );
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

	ball->rect.x = message.GetXPos() * remoteResolutionScale;

	// Need to add ball's height, because ball it traveling in oposite direction.
	// The board is also flipped, so the ball will have the oposite horizontal collision edge.
	//ball->rect.y = ( message.GetYPos() - ball->rect.h ) * remoteResolutionScale;
	ball->rect.y = ( message.GetYPos()  * remoteResolutionScale ) - ball->rect.h ;


	ball->SetDirection( Vector2f( message.GetXDir(), message.GetYDir() ) );
}
void GameManager::RecieveBallKillMessage( const TCPMessage &message )
{
	//PrintRecv( message );
	if ( ballList.size() > 0 )
	{
		RemoveBall( GetBallFromID( message.GetObjectID() ));

	}
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
	double xPos = message.GetXPos();
	if ( xPos > 0 && xPos < windowSize.w )
	{
		remotePaddle->rect.x = xPos;
	}
}
void GameManager::SendPaddlePosMessage( )
{
	if ( !isTwoPlayerMode || !netManager.IsConnected()  || !isResolutionScaleRecieved )
		return;

	// Sending
	TCPMessage msg;

	std::stringstream ss;
	msg.SetMessageType( MessageType::PaddlePosition );
	msg.SetXPos( localPaddle->rect.x );
	ss << msg;
	netManager.SendMessage( ss.str() );

	PrintSend(msg);
}

void GameManager::SendGameSettingsMessage()
{
	if ( !isTwoPlayerMode )
		return;

	TCPMessage msg;

	std::stringstream ss;

	msg.SetMessageType( MessageType::GameSettings );
	msg.SetObjectID( 0 );

	msg.SetXSize( windowSize.w );
	msg.SetYSize( windowSize.h );

	msg.SetBoardScale( scale  );

	ss << msg;
	netManager.SendMessage( ss.str() );

	PrintSend(msg);
}
void GameManager::SendBallSpawnMessage( const std::shared_ptr<Ball> &ball)
{
	if ( !isTwoPlayerMode )
		return;

	TCPMessage msg;

	Rect r = ball->rect;
	std::stringstream ss;

	msg.SetMessageType( MessageType::BallSpawned );
	msg.SetObjectID( ball->GetBallID() );

	msg.SetXPos( r.x );
	msg.SetYPos( windowSize.h - r.y );

	msg.SetXDir( ball->GetDirection().x );
	msg.SetYDir( ball->GetDirection().y * -1.0 );

	ss << msg;
	netManager.SendMessage( ss.str() );

	//PrintSend(msg);
}
void GameManager::SendBallDataMessage( const std::shared_ptr<Ball> &ball)
{
	if ( !isTwoPlayerMode )
		return;

	TCPMessage msg;

	Rect r = ball->rect;
	std::stringstream ss;

	msg.SetMessageType( MessageType::BallData );

	msg.SetObjectID(  ball->GetBallID() );
	msg.SetXPos( r.x );
	msg.SetYPos(  windowSize.h - r.y );

	msg.SetXDir( ball->GetDirection().x );
	msg.SetYDir( ball->GetDirection().y * -1.0 );

	ss << msg;
	netManager.SendMessage( ss.str() );

	//PrintSend(msg);
}
void GameManager::SendBallKilledMessage( const std::shared_ptr<Ball> &ball)
{
	if ( !isTwoPlayerMode )
		return;

	TCPMessage msg;
	std::stringstream ss;

	msg.SetMessageType( MessageType::BallKilled );
	msg.SetObjectID(  ball->GetBallID() );

	ss << msg;
	netManager.SendMessage( ss.str() );

	PrintSend(msg);
}
void GameManager::SendTileHitMessage( unsigned int tileID )
{
	if ( !isTwoPlayerMode )
		return;

	TCPMessage msg;
	std::stringstream ss;

	msg.SetMessageType( MessageType::TileHit );
	msg.SetObjectID( tileID );

	ss << msg;
	netManager.SendMessage( ss.str() );
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
	auto isDeadFunc = [=]( std::shared_ptr< Ball > ball )
	{
		if ( ball->IsAlive() )
			return false;

		RemoveBall( ball );
		return true;
	};

	auto newEnd = std::remove_if( ballList.begin(), ballList.end(), isDeadFunc );

	// Remove item returned by remove_if
	ballList.erase( newEnd, ballList.end( ) );
}
std::shared_ptr< Ball > GameManager::GetBallFromID( unsigned int ID )
{
	for ( auto p : ballList )
	{
		if ( ID == p->GetBallID() )
		{
			return p;
		}
	}

	return nullptr;
}
std::shared_ptr< Tile > GameManager::GetTileFromID( unsigned int ID )
{
	for ( auto p : tileList )
	{
		if ( ID == p->GetTileID() )
			return p;
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
	if ( menuManager.HasGameStateChanged() )
	{
		renderer.SetGameState( menuManager.GetGameState() );

		if ( menuManager.GetGameState() == GameState::Quit )
			runGame = false;
		else if ( menuManager.GetGameState() == GameState::InGame && menuManager.GetPrevGameState() != GameState::Paused )
		{
			Restart();
		}
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
	if ( menuManager.GetGameState() == GameState::InGame && !netManager.IsServer() )
	{
		SetLocalPaddlePosition( buttonEvent.x, buttonEvent.y );
	}

	else if ( menuManager.GetGameState() == GameState::MainMenu  || menuManager.GetGameState() == GameState::Paused )
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
	if ( event.type == SDL_KEYDOWN )
	{
		switch  ( event.key.keysym.sym )
		{
			case SDLK_F11:
				renderer.ToggleFullscreen();
				break;
			case SDLK_RETURN:
			case SDLK_b:
				//AddBall( Player::Remote );
				AddBall( Player::Local, ballCount );
				break;
			case SDLK_ESCAPE:
				//menuManager.GoBackToPreviousMenuState();
				menuManager.GoToMenu();
				break;
			case SDLK_l:
				++localPlayerLives;
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
			case SDLK_c:
				SendGameSettingsMessage();
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
	if ( menuManager.GetGameState() != GameState::InGame )
	{
		renderer.Render( );
		return;
	}

	if ( netManager.IsServer() )
		AIMove();

	if ( isTwoPlayerMode )
		UpdateNetwork();

	UpdateBalls( delta );
	UpdateBonusBoxes( delta );

	if ( IsLevelDone() )
	{
		std::cout << "GameMAanager@" << __LINE__ << " Level is done..\n";
		GenerateBoard();
	}

	UpdateGUI();
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

	if ( !ball->TileCheck( tile->rect, tile->GetTileID() ) )
		return;

	if ( ball->GetOwner() == Player::Local ) SendBallDataMessage( ball );

	tile->Hit();

	SendTileHitMessage( tile->GetTileID() );

	UpdateTileHit( ball, tile );
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
				++localPlayerLives;
				p->Kill();
			}
		}
		else
		{
			if ( p->rect.CheckTileIntersection( remotePaddle->rect ) )
			{
				++remotePlayerLives;
				p->Kill();
			}
		}
	}

	MoveBonusBoxes ( delta );
	RemoveDeadBonusBoxes();
}

void GameManager::MoveBonusBoxes ( double delta )
{
	auto func = [ = ] ( std::shared_ptr< BonusBox > curr )
	{
		Vector2f direction = curr->GetDirection();

		curr->rect.x += direction.x * delta * 0.7;
		curr->rect.y += direction.y * delta * 0.7;

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
void GameManager::UpdateGUI( )
{
	if ( IsLevelDone() && !boardLoader.IsLastLevel() )
	{
		if ( localPlayerPoints > remotePlayerPoints )
			renderer.RenderText( "Yay, you won :D", Player::Local  );
		else
			renderer.RenderText( "Oh no, you lost :\'(", Player::Local  );
	}
	else if ( localPlayerActiveBalls == 0 )
	{
		if ( localPlayerLives == 0 )
			renderer.RenderText( "Game Over", Player::Local  );
		else
			renderer.RenderText( "Press enter to launch ball", Player::Local  );
	}

	renderer.RenderPoints   ( localPlayerPoints, Player::Local );
	renderer.RenderLives    ( localPlayerLives, Player::Local );
	renderer.RenderBallCount( localPlayerActiveBalls, Player::Local );

	renderer.RenderLives    ( remotePlayerLives, Player::Remote );
	renderer.RenderPoints   ( remotePlayerPoints, Player::Remote );
	renderer.RenderBallCount( remotePlayerActiveBalls, Player::Remote );

	renderer.Render( );
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

	if ( !boardLoader.IsLastLevel() )
	{
		std::cout << "GameMAanager@" << __LINE__ << " Last level..\n";
		return;
	}

	Board b = boardLoader.GenerateBoard( windowSize );
	std::vector<TilePosition> vec = b.GetTiles();

	for ( const auto &tile : vec )
		AddTile( tile.xPos, tile.yPos, tile.type );

	if ( netManager.IsServer() )
		SetScale( b.GetScale() );
}
bool GameManager::IsLevelDone()
{
	if ( menuManager.GetGameState() == GameState::InGame )
	{
		auto IsTileDestroyable = []( const std::shared_ptr< Tile > &tile ){ return ( tile->GetTileType() != TileType::Unbreakable ); };
		return std::count_if( tileList.begin(), tileList.end(), IsTileDestroyable )  == 0;
	}

	return false;
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
void GameManager::CreateMenu()
{
	menuManager.AddMenuElememts( renderer );
	menuManager.AddPauseMenuElememts( renderer );
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
	std::cout << "Scale : " << scale_ << std::endl;
	scale = scale_;
	ApplyScale();
	SendGameSettingsMessage();
}
void GameManager::ApplyScale( )
{
	for ( auto& p : tileList )
	{
		if ( scale < 1.0 )
		{
			p->rect.x = ( p->rect.x * scale ) + ( ( windowSize.w - ( windowSize.w * scale ) ) * 0.5 );
			p->rect.y = ( p->rect.y * scale ) + ( ( windowSize.h - ( windowSize.h * scale ) ) * 0.5 );
		}

		p->ResetScale();
		p->SetScale( scale );
	}

	for ( auto& p : ballList )
	{
		p->ResetScale();
		p->SetScale( scale );
	}
}
