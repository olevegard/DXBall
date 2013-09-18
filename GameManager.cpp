#include "GameManager.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"

#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>

#include <random>

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

	if ( !renderer.Init( windowSize, startFS ) )
		return false;

	std::cout << localPlayerName << " vs " << remotePlayerName << std::endl;

	Restart();
	renderer.RenderPlayerCaption( localPlayerName, Player::Local );
	renderer.RenderPlayerCaption( remotePlayerName, Player::Remote );

	renderer.RenderLives ( localPlayerLives , Player::Local );
	renderer.RenderPoints( localPlayerPoints, Player::Local );

	renderer.RenderLives ( remotePlayerLives , Player::Remote );
	renderer.RenderPoints( remotePlayerPoints, Player::Remote );

	return true;
}

void GameManager::Restart()
{
	std::cout << "Restart\n";
	localPaddle = std::make_shared< Paddle > ();
	localPaddle->textureType = GamePiece::Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h - ( localPaddle->rect.h * 1.5 );
	renderer.SetLocalPaddle( localPaddle );

	if ( isTwoPlayerMode )
	{
		remotePaddle = std::make_shared< Paddle > ();
		remotePaddle->textureType = GamePiece::Paddle;
		remotePaddle->rect.w = 120;
		remotePaddle->rect.h = 30;
		remotePaddle->rect.x = 400;
		remotePaddle->rect.y = remotePaddle->rect.h * 0.5;
		renderer.SetRemotePaddle( remotePaddle );
	}

	tileCount = 0;

	GenerateBoard();

	localPlayerPoints = 0;
	localPlayerLives = 3;
	localPlayerActiveBalls = 0;

	remotePlayerPoints = 0;
	remotePlayerLives = 3;
	remotePlayerActiveBalls = 0;

	renderer.RenderLives( localPlayerLives, Player::Local );
	renderer.RenderPoints( localPlayerPoints, Player::Local );

	renderer.RenderLives( remotePlayerLives, Player::Remote );
	renderer.RenderPoints( remotePlayerPoints, Player::Remote );

	renderer.RenderText( "Press enter to start", Player::Local );
}

void GameManager::AddBall( Player owner )
{
	if ( owner == Player::Local )
	{
		if (  localPlayerLives == 0 )
		{
			return;
		}
		++localPlayerActiveBalls;
	}
	else  if ( owner == Player::Remote )
	{
		if (  remotePlayerLives == 0 )
		{
			return;
		}
		++remotePlayerActiveBalls;
	}

	std::shared_ptr< Ball > ball = std::make_shared< Ball >(  );
	ball->textureType = GamePiece::Ball;
	ball->SetOwner( owner );

	ballList.push_back( ball );
	renderer.AddBall( ball );

}

void GameManager::RemoveBall( const std::shared_ptr< Ball >  ball )
{
	if ( ball->GetOwner() == Player::Local )
	{
		--localPlayerActiveBalls;

		if ( localPlayerActiveBalls == 0 )
			--localPlayerLives;
	} else if ( ball->GetOwner() == Player::Remote )
	{
		--remotePlayerActiveBalls;

		if ( remotePlayerActiveBalls == 0 )
			--remotePlayerLives;
	}

	renderer.RemoveBall( ball );

}
void GameManager::AddTile( short posX, short posY, TileTypes tileType )
{
	std::shared_ptr< Tile > tile = std::make_shared< Tile >( tileType, tileCount++ );
	tile->textureType = GamePiece::Tile;
	tile->rect.x = posX;
	tile->rect.y = posY;
	tile->rect.w = 60;
	tile->rect.h = 20;

	tileList.push_back( tile );

	renderer.AddTile( tile );
}

void GameManager::RemoveTile( std::shared_ptr< Tile > tile )
{
	renderer.RemoveTile( tile );

	// Decrement tile count
	--tileCount;
}

void GameManager::AddBonusBox( const std::shared_ptr< Ball > &triggerBall, double x, double y )
{
	if ( GenRandomNumber( 1 ) != 0 )
		return;

	std::shared_ptr< BonusBox > bonusBox  = std::make_shared< BonusBox > ();
	bonusBox->rect.x = x;
	bonusBox->rect.y = y;

	Player ballOwner = triggerBall->GetOwner();
	Vector2f direction = triggerBall->GetDirection();

	if ( ballOwner == Player::Local )
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
		p->BoundCheck( windowSize );

		if ( p->GetOwner() == Player::Local )
		{
			p->PaddleCheck( localPaddle->rect );
		}
		else if ( isTwoPlayerMode && p->GetOwner() == Player::Remote )
		{
			p->PaddleCheck( remotePaddle->rect );
		}

		CheckBallTileIntersection( p );

		if ( p->DeathCheck( windowSize ) )
		{
			p->Kill();
		}
	}

	DeleteDeadBalls();
}
void GameManager::DeleteDeadBalls()
{
	auto isDeadFunc = [&]( std::shared_ptr< Ball > ball )
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

void GameManager::Run()
{
	bool quit = false;
	SDL_Event event;

	double tileWidth = localPaddle->rect.w;
	double halfTileWidth = tileWidth / 2;

	unsigned int ticks;
	while ( !quit )
	{
		bool delay1 = false;
		bool delay2 = false;
		bool delay3 = false;
		bool delay4 = false;

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
						AddBall( Player::Remote );
						AddBall( Player::Local );
						break;
					case SDLK_q:
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_r:
						++localPlayerLives;
						break;
					case SDLK_c:
						std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
						break;
					case SDLK_t:
						std::cout << "Tile respawned\n";
						GenerateBoard();
						break;
					case SDLK_1:
						std::cout << "Delay added\n";
						delay1 = true;
						break;
					case SDLK_2:
						std::cout << "Delay added\n";
						delay2 = true;
						break;
					case SDLK_3:
						std::cout << "Delay added\n";
						delay3 = true;
						break;
					case SDLK_4:
						std::cout << "Delay added\n";
						delay4 = true;
						break;
					case SDLK_p:
						//std::cin.ignore();
						break;
					default:
						break;
				}
			}

			if ( event.type == SDL_WINDOWEVENT)
			{
				if ( SDL_WINDOWEVENT_LEAVE )
					renderer.ForceMouseFocus();
			}

			if ( event.motion.x != 0 && event.motion.y != 0 )
				localPaddle->rect.x = static_cast< double > ( event.motion.x ) - halfTileWidth;

			if ( ( localPaddle->rect.x + tileWidth ) > windowSize.w )
				localPaddle->rect.x = static_cast< double > ( windowSize.w ) - tileWidth;

			if ( localPaddle->rect.x  <= 0  )
				localPaddle->rect.x = 0;
		}

		AIMove();

		double delta = timer.GetDelta( );
		//std::cout << "Delta : " << delta << std::endl;
		UpdateBalls( delta );
		UpdateBonusBoxes( delta );
		UpdateGUI();

		unsigned int diff = SDL_GetTicks() - ticks;

		if ( fpsLimit > 0 && diff < frameDuration )
		{
			unsigned short delay = static_cast< unsigned short > ( ( frameDuration  - diff  ) + 0.5 );

			if ( diff < 60 )
			{
				SDL_Delay( delay );
			}
		}
		/*
		if ( delay1 )
			SDL_Delay( 1 );
		if ( delay2 )
			SDL_Delay( 2 );
		if ( delay3 )
			SDL_Delay( 5 );
		if ( delay4 )
			SDL_Delay( 10 );
		*/
	}

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

		double deadCenter = ( (*highest)->rect.x + (*highest)->rect.w / 2 )  - ( ( localPaddle->rect.w / 2.0) * GenRandomNumber( -1.0, 1.0 ) );
		remotePaddle->rect.x = deadCenter;
	}
}
void GameManager::CheckBallTileIntersection( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > closestTile = FindClosestIntersectingTile( ball );
	RemoveClosestTile( ball, closestTile );
}
void GameManager::RemoveClosestTile( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile )
{
	auto itClosestTile = std::find( tileList.begin(), tileList.end(), tile );
	if ( tile )
	{
		if ( !ball->TileCheck( tile->rect, tile->GetTileID() ) )
			return;

		tile->Hit();

		bool isDestroyed = tile->IsDestroyed();
		IncrementPoints( tile->GetTileTypeAsIndex(), isDestroyed, ball->GetOwner() );
		if ( isDestroyed )
		{
			AddBonusBox( ball, tile->rect.x, tile->rect.y );

			if (tile->GetTileType() == TileTypes::Explosive )
				HandleExplosions( tile, ball->GetOwner() );
			else
			{
				tileList.erase( itClosestTile );
				RemoveTile( tile );
			}
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
void GameManager::HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner )
{
	std::vector< Rect > rectVec = GenereateExplosionRects( explodingTile );

	auto isDeadFunc = [&]( std::shared_ptr< Tile > curr )
	{
		if ( !Rect::CheckTileIntersection( rectVec, curr->rect) )
			return false;

		IncrementPoints( curr->GetTileTypeAsIndex(), true, ballOwner );
		renderer.RemoveTile( curr );

		return true;
	};

	auto newEnd = std::remove_if( tileList.begin(), tileList.end(), isDeadFunc );

	// Remove item returned by remove_if
	tileList.erase( newEnd, tileList.end( ) );
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
			if ( Rect::CheckTileIntersection( explodedTileRects, p->rect) )
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
		return  ( tile->GetTileType() == TileTypes::Explosive ) &&
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
	auto func = [ delta ]( std::shared_ptr< BonusBox > curr )
	{
		Vector2f direction = curr->GetDirection();

		curr->rect.x += direction.x * delta * 0.7;
		curr->rect.y += direction.y * delta * 0.7;

		return curr;
	};

	std::transform( bonusBoxList.begin(), bonusBoxList.end(), bonusBoxList.begin() , func );

}
void GameManager::RemoveDeadBonusBoxes()
{
	auto isDeadFunc = [&]( std::shared_ptr< BonusBox > curr )
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

	renderer.RenderPoints( localPlayerPoints, Player::Local );
	renderer.RenderLives( localPlayerLives, Player::Local );


	renderer.RenderLives( remotePlayerLives, Player::Remote );
	renderer.RenderPoints( remotePlayerPoints, Player::Remote );

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
	short x = 60;
	short y = 200;

	short countRows = 15;
	short tileHeight = 25;
	short boardHeight = ( countRows * tileHeight );
	short totalVerticalFreeSpace = static_cast< short > (windowSize.h - boardHeight  + 20 );
	short halfVerticalFreeSpace = static_cast< short > ( totalVerticalFreeSpace / 2 );

	y = halfVerticalFreeSpace;

	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );//Middle
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;


	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Hard );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );

	x = 60;
	y += 25;

	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Regular );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );

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
double GameManager::GenRandomNumber( double min, double max ) const
{
	std::random_device rseed;
	std::mt19937 rgen(rseed());
	std::uniform_real_distribution<double> rdist( min, max );

	return rdist(rgen);
}
int GameManager::GenRandomNumber( int max ) const
{
	std::random_device rseed;
	std::mt19937 rgen(rseed());
	std::uniform_int_distribution<int> rdist( 0, max );

	return rdist(rgen);
}
