
#include "GameManager.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"
#include "BoardLoader.h"

#include "math/Math.h"
#include "math/RectHelpers.h"
#include "TCPMessage.h"
#include "enums/MessageType.h"

#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <cmath>


	GameManager::GameManager()
	:	renderer()
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

	,	points{ 20, 50, 100, 500 }
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

	return true;
}

void GameManager::SetIsServer( bool isServer )
{
	netManager.Init( isServer );
	boardLoader.SetIsServer( isServer );
}
void GameManager::Restart()
{
	std::cout << "Restart\n";
	localPaddle = std::make_shared< Paddle > ();
	localPaddle->textureType = TextureType::e_Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h - ( localPaddle->rect.h * 1.5 );
	renderer.SetLocalPaddle( localPaddle );

	if ( isTwoPlayerMode )
	{
		remotePaddle = std::make_shared< Paddle > ();
		remotePaddle->textureType = TextureType::e_Paddle;
		remotePaddle->rect.w = 120;
		remotePaddle->rect.h = 30;
		remotePaddle->rect.x = 400;
		remotePaddle->rect.y = remotePaddle->rect.h * 0.5;
		renderer.SetRemotePaddle( remotePaddle );
	}

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
	tile->rect.x = posX;
	tile->rect.y = posY;
	tile->rect.w = 60;
	tile->rect.h = 20;

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
		std::stringstream ss;

		// Reading
		bool stop = false;
		while ( !stop )
		{
			std::string str =netManager.ReadMessage(); 
			if ( str == "" )
			{
				stop = true;
				break;
			}

			ss.str( str );
			while ( ss >> msg )
			{

				if ( msg.msgType == MessageType::PaddlePosition )
				{
					//PrintRecv( msg );
					if ( msg.xPos > 0 && msg.xPos < windowSize.w )
					{
						remotePaddle->rect.x = msg.xPos;
					}
				}
				else if ( msg.msgType == MessageType::BallSpawned )
				{
					//PrintRecv( msg );
					std::shared_ptr< Ball > ball = AddBall( Player::Remote, msg.objectID );
					ball->rect.x = msg.xPos;
					ball->rect.y = msg.yPos;
					ball->SetDirection( Vector2f( msg.xDir, msg.yDir ) );
				}else if ( msg.msgType == MessageType::BallData )
				{
					PrintRecv( msg );
					if ( ballList.size() > 0 )
					{
						std::shared_ptr< Ball > ball = GetBallFromID( msg.objectID );

						if ( !ball )
						{
							continue;
						}

						ball->rect.x = msg.xPos;
						ball->rect.y = msg.yPos;
						//double distX = ( ball->rect.x - msg.xPos);
						//double distY = ( ball->rect.y - msg.yPos);
						//std::cout << "Dist : " << distX << " , " << distY << std::endl;

						ball->SetDirection( Vector2f( msg.xDir, msg.yDir ) );
					}
				} else if ( msg.msgType == MessageType::BallKilled )
				{
					//PrintRecv( msg );
					if ( ballList.size() > 0 )
					{
						RemoveBall( GetBallFromID( msg.objectID ));

					}
				} else if ( msg.msgType == MessageType::TileHit )
				{
					PrintRecv( msg );
					if ( tileList.size() > 0 )
					{
						std::shared_ptr< Tile > tile = GetTileFromID( msg.objectID );
						tile->Hit();

						if ( tile->IsDestroyed() )
						{
							auto itClosestTile = std::find( tileList.begin(), tileList.end(), tile );
							tileList.erase( itClosestTile );
							RemoveTile( tile );
						}
					}
				}  else
				{
					std::cout << __LINE__ << " : UpdateNetwork message received " << msg << std::endl;
				}
			}
		}
	}

}

void GameManager::SendPaddlePosMessage( )
{
	// Sending
	//
	TCPMessage msg;

	std::stringstream ss;
	msg.msgType = MessageType::PaddlePosition;
	msg.xPos = localPaddle->rect.x;
	ss << msg;
	netManager.SendMessage( ss.str() );

	//PrintSend(msg);
}
void GameManager::SendBallSpawnMessage( const std::shared_ptr<Ball> &ball)
{
	TCPMessage msg;

	Rect r = ball->rect;
	std::stringstream ss;

	msg.msgType = MessageType::BallSpawned;
	msg.objectID = ball->GetBallID();

	msg.xPos = r.x;
	msg.yPos = windowSize.h - r.y;

	msg.xDir = ball->GetDirection().x;
	msg.yDir = ball->GetDirection().y * -1.0;

	ss << msg;
	netManager.SendMessage( ss.str() );

	//PrintSend(msg);
}
void GameManager::SendBallDataMessage( const std::shared_ptr<Ball> &ball)
{
	TCPMessage msg;

	Rect r = ball->rect;
	std::stringstream ss;

	msg.msgType = MessageType::BallData;

	msg.objectID = ball->GetBallID();
	msg.xPos = r.x;
	msg.yPos = windowSize.h - r.y;

	msg.xDir = ball->GetDirection().x;
	msg.yDir = ball->GetDirection().y * -1.0;

	ss << msg;
	netManager.SendMessage( ss.str() );

	//PrintSend(msg);
}
void GameManager::SendBallKilledMessage( const std::shared_ptr<Ball> &ball)
{
	TCPMessage msg;
	std::stringstream ss;

	msg.msgType = MessageType::BallKilled;
	msg.objectID = ball->GetBallID();

	ss << msg;
	netManager.SendMessage( ss.str() );

	//PrintSend(msg);
}
void GameManager::SendTileHitMessage( unsigned int tileID )
{
	TCPMessage msg;
	std::stringstream ss;

	msg.msgType = MessageType::TileHit;
	msg.objectID = tileID;

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
	//auto func = [ID]( std::shared_ptr< Ball > p){ return ID == p->GetBallID();  };
	//auto it = std::find_if( ballList.begin(), ballList.end(), func );
	for ( auto p : ballList )
	{
		if ( ID == p->GetBallID() )
		{
			return p;
		}
	}

	return nullptr;

	//if ( it == ballList.end() ) return nullptr;

	//return (*it);
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
	bool quit = false;
	SDL_Event event;

	unsigned int ticks;

	while ( !quit )
	{
		ticks = SDL_GetTicks();

		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
				quit = true;

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
						ClearBoard();
						break;
					default:
						break;
				}
			}

			if ( event.type == SDL_WINDOWEVENT)
			{
				//if ( SDL_WINDOWEVENT_LEAVE ) renderer.ForceMouseFocus();
			}
			if ( event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP )
				HandleMouseEvent( event.button );
		}
		if ( menuManager.HasGameStateChanged() )
		{
			renderer.SetGameState( menuManager.GetGameState() );
			if ( menuManager.GetGameState() == GameState::Quit )
				quit = true;
			else if ( menuManager.GetGameState() == GameState::InGame && menuManager.GetPrevGameState() != GameState::Paused )
			{
				Restart();
			}
		}

		Update( timer.GetDelta( ) );

		DoFPSDelay( ticks );

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


	//AIMove();
	if ( netManager.IsServer() )
		AIMove_Local();

	UpdateNetwork();
	UpdateBalls( delta );
	UpdateBonusBoxes( delta );

	if ( IsLevelDone() )
	{
		std::cout << "Level is done..\n";
		GenerateBoard();
	}

	UpdateGUI();
}
void GameManager::AIMove()
{
	if ( !remotePaddle )
		return;

	if ( ballList.size() == 0 )
		return;

	std::vector< std::shared_ptr< Ball > >::iterator highest = ballList.end();

	if ( ballList.size() == 1 )
		highest = ballList.begin();
	else
	{
		// Returning true means ball1 gets selected
		auto compareBallHeights = []( std::shared_ptr< Ball > ball1, std::shared_ptr< Ball > ball2  )
		{
			if ( ball2->GetOwner() == Player::Local )
				return true;

			if ( ball1->GetOwner() == Player::Local )
				return false;

			return ( ball1->rect.y < ball2->rect.y);
		};
		highest = std::min_element( ballList.begin(), ballList.end(), compareBallHeights );
	}

	if ( highest != ballList.end() )
	{
		if ( (*highest)->rect.y > ( remotePaddle->rect.y + remotePaddle->rect.h ))
			return;

		double deadCenter = ( (*highest)->rect.x + (*highest)->rect.w / 2 )  - ( ( localPaddle->rect.w / 2.0) * Math::GenRandomNumber( -1.0, 1.0 ) );
		remotePaddle->rect.x = deadCenter;
	}
}
void GameManager::AIMove_Local()
{
	if ( !localPaddle )
		return;

	if ( ballList.size() == 0 )
		return;

	double yMax = 0;
	std::shared_ptr< Ball > highest;
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

	if ( highest == nullptr )
		return;

	if ( highest->GetOwner() == Player::Remote )
		return;

	if ( ( highest->rect.y + highest->rect.h ) <  localPaddle->rect.y  )
		return;

	double deadCenter = ( highest->rect.x + highest->rect.w / 2 )  - ( ( localPaddle->rect.w / 2.0) * Math::GenRandomNumber( -1.0, 1.0 ) );

	localPaddle->rect.x = deadCenter;
	SendPaddlePosMessage();
}
void GameManager::CheckBallTileIntersection( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > closestTile = FindClosestIntersectingTile( ball );
	RemoveClosestTile( ball, closestTile );
}
void GameManager::RemoveClosestTile( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile )
{
	if ( tile )
	{
		if ( !ball->TileCheck( tile->rect, tile->GetTileID() ) )
			return;

		if ( ball->GetOwner() == Player::Local ) SendBallDataMessage( ball );

		tile->Hit();

		SendTileHitMessage( tile->GetTileID() );

		UpdateTileHit( ball, tile );
	}
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
void GameManager::RemoveDeadTiles()
{

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
	if ( localPlayerActiveBalls == 0 )
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
	std::vector<TilePosition> vec = boardLoader.GenerateBoard( windowSize );

	for ( const auto &tile : vec )
		AddTile( tile.xPos, tile.yPos, tile.type );
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

