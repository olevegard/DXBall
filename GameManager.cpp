#include "GameManager.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"

#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>

	GameManager::GameManager()
	:	renderer()
	,	timer()
	,	localPaddle()
	,	localPlayerPoints( 0 )
	,	localPlayerLives( 3 )
	,	localPlayerActiveBalls( 0 )
	,	ballList()
	,	windowSize()

	,	points{ 20, 50, 100, 200 }
	,	tileCount( 0 )
	,	fpsLimit( 60 )
	,	frameDuration( 1000.0 / 60.0 )
{
	windowSize.x = 0.0;
	windowSize.y = 0.0;
	windowSize.w = 1920 / 2;
	windowSize.h = 1080 / 2;
}

int GameManager::Init( const std::string &localPlayerName, const std::string &remotePlayerName, const SDL_Rect &size, bool startFS )
{
	windowSize = size;

	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
		return 1;

	if ( !renderer.Init( windowSize, startFS ) )
		return 1;

	renderer.RenderPlayerCaption( localPlayerName, Player::Local );
	renderer.RenderPlayerCaption( remotePlayerName, Player::Remote );

	renderer.RenderLives ( 0, Player::Remote );
	renderer.RenderPoints( 1, Player::Remote );

	Restart();

	return 0;
}

void GameManager::Restart()
{
	localPaddle = std::make_shared< Paddle > ();
	localPaddle->textureType = GamePiece::Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h  - localPaddle->rect.h;
	renderer.SetLocalPaddle( localPaddle );

	tileCount = 0;
	GenerateBoard();

	localPlayerPoints = 0;
	localPlayerLives = 3;
	localPlayerActiveBalls = 0;

	renderer.RenderLives( 1, Player::Local );
	renderer.RenderPoints( localPlayerPoints, Player::Local );
	renderer.RenderText( "Press enter to start", Player::Local );
}

void GameManager::AddBall()
{
	if ( localPlayerActiveBalls > 0 || localPlayerLives == 0 )
	{
		return;
	}

	std::shared_ptr< Ball > ball = std::make_shared< Ball >(  );
	ball->textureType = GamePiece::Ball;
	ball->SetOwner( 0 );

	ballList.push_back( ball );
	renderer.AddBall( ball );

	++localPlayerActiveBalls;
}

void GameManager::RemoveBall( const std::shared_ptr< Ball >  ball )
{
	renderer.RemoveBall( ball );
	--localPlayerActiveBalls;

	if ( localPlayerActiveBalls == 0 )
		--localPlayerLives;
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

	std::cout << "Adding Tile at : " << posX << " , " << posY << std::endl;

	renderer.AddTile( tile );
}

std::vector< std::shared_ptr< Tile > >::iterator GameManager::RemoveTile( std::shared_ptr< Tile > tile )
{
	std::vector< std::shared_ptr< Tile > >::iterator p = std::find( tileList.begin(), tileList.end(), tile );

	std::cout << "Removing tile at     : " << tile->rect << std::endl;

	if ( p != tileList.end() )
		p = tileList.erase( p );

	renderer.RemoveTile( tile );

	// Decrement tile count
	--tileCount;

	return p;
}

void GameManager::UpdateBalls( double delta )
{
	renderer.RenderLives( ballList.size(), Player::Local  );

	if ( ballList.size() > 0 )
	{
		renderer.RemoveText();

		for ( auto p = ballList.begin(); p != ballList.end() ;  )
		{
			(*p)->Update( delta );
			(*p)->BoundCheck( windowSize );
			(*p)->PaddleCheck( localPaddle->rect );

			CheckBallTileIntersection( *p );

			if ( (*p)->DeathCheck( windowSize ) )
			{
				//(*p)->rect.x = 200;
				//(*p)->rect.y = 20;
				RemoveBall( (*p) );
				p = ballList.erase( p );
			} else
			{
				++p;
			}
		}
	} else 
		renderer.RenderText( "Press enter to launch ball", Player::Local );
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
						AddBall();
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
						std::cin.ignore();
						break;
					default:
						break;
				}
			}

			if ( event.motion.x != 0 && event.motion.y != 0 )
				localPaddle->rect.x = static_cast< double > ( event.motion.x ) - halfTileWidth;

			if ( ( localPaddle->rect.x + tileWidth ) > windowSize.w )
				localPaddle->rect.x = static_cast< double > ( windowSize.w ) - tileWidth;

			if ( localPaddle->rect.x  <= 0  )
				localPaddle->rect.x = 0;

		}
		double delta = timer.GetDelta( );
		UpdateBalls( delta );
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
void GameManager::CheckBallTileIntersection( std::shared_ptr< Ball > ball )
{
	std::shared_ptr< Tile > tile;
	std::shared_ptr< Tile > closestTile;
	double closest = std::numeric_limits< double >::max();
	double current = current;
	auto itClosestTile = tileList.end();
	for ( auto p = tileList.begin(); p != tileList.end() && (*p) != nullptr; ++p  )
	{
		tile = (*p);
		if ( ball->CheckTileSphereIntersection( tile->rect, ball->rect, current ) )
		{
			if ( current < closest )
			{
				closest = current;
				closestTile = tile;
				itClosestTile = p;
			}
			break;
		} else
		{
			//++p;
		}
	}

	if ( tile )
	{
		if ( !ball->TileCheck( tile->rect, tile->GetTileID() ) )
			return;

		localPlayerPoints += 10;

		tile->Hit();

		if ( tile->IsDestroyed() )
		{
			if (tile->GetTileType() == TileTypes::Explosive )
				CheckExplosions( tile );

			localPlayerPoints += points[ tile->GetTileTypeAsIndex() ];
			if ( tile->GetTileType() != TileTypes::Explosive )
				RemoveTile( tile );
		}
	}

}
void GameManager::CheckExplosions( std::shared_ptr< Tile > explodingTile )
{
	Rect explodeRect( explodingTile->rect );
	//std::cout << "Before :" << explodeRect << std::endl;
	explodeRect.x -= explodeRect.w;
	explodeRect.y -= explodeRect.h;
	explodeRect.w += explodeRect.w * 2.0;
	explodeRect.h += explodeRect.h * 2.0;
	//std::cout << "After :" << explodeRect << std::endl;

	std::vector < std::shared_ptr< Tile > > explodelist;
	double distMin = std::numeric_limits< double >::max();
	std::shared_ptr< Tile > closestExplosiveTile;
	bool b = false;
	for ( auto p = tileList.begin(); p != tileList.end() ;  )
	{
		if ( (*p) != explodingTile && explodeRect.CheckTileIntersection( (*p)->rect ) )
		{
			if ( (*p)->GetTileType() == TileTypes::Explosive && !b )
			{ 
				double dist = explodeRect.FindDistanceBetweenTiles( (*p)->rect );

				if ( dist < distMin )
				{
					distMin = dist;
					closestExplosiveTile = (*p);
				}
			}

			p = RemoveTile( (*p) );

		} else
		{
			++p;
		}
	}

	if ( closestExplosiveTile )
	{
		//std::cout << "Closest : " << closestExplosiveTile->rect << std::endl;
		CheckExplosions( closestExplosiveTile );
		//std::cin.ignore();
	}

}
void GameManager::UpdateGUI( )
{
	if ( localPlayerActiveBalls == 0 )
	{
		if ( localPlayerLives == 0 )
			renderer.RenderText( "Game Over", Player::Local  );
		else
			renderer.RenderText( "Press Enter to launch ball", Player::Local  );
	}

	renderer.RenderPoints( localPlayerPoints, Player::Local );
	renderer.RenderLives( localPlayerLives, Player::Local );
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
	//================================ Explosive test
	AddTile( 325, 425, TileTypes::Unbreakable );
	AddTile( 390, 425, TileTypes::Hard );
	AddTile( 455, 425, TileTypes::Explosive );
	AddTile( 520, 425, TileTypes::Hard );
	AddTile( 585, 425, TileTypes::Unbreakable );

	AddTile( 325, 400, TileTypes::Unbreakable );
	AddTile( 390, 400, TileTypes::Hard );
	AddTile( 455, 400, TileTypes::Hard );
	AddTile( 520, 400, TileTypes::Hard );
	AddTile( 585, 400, TileTypes::Unbreakable );

	AddTile( 325, 375, TileTypes::Unbreakable );
	AddTile( 390, 375, TileTypes::Hard );
	AddTile( 455, 375, TileTypes::Explosive );
	AddTile( 520, 375, TileTypes::Hard );
	AddTile( 585, 375, TileTypes::Unbreakable );

	AddTile( 325, 350, TileTypes::Unbreakable );
	AddTile( 390, 350, TileTypes::Hard );
	AddTile( 455, 350, TileTypes::Explosive );
	AddTile( 520, 350, TileTypes::Hard );
	AddTile( 585, 350, TileTypes::Unbreakable );

	AddTile( 325, 325, TileTypes::Unbreakable );
	AddTile( 390, 325, TileTypes::Hard );
	AddTile( 455, 325, TileTypes::Explosive );
	AddTile( 520, 325, TileTypes::Hard );
	AddTile( 585, 325, TileTypes::Unbreakable );

	AddTile( 325, 300, TileTypes::Unbreakable );
	AddTile( 390, 300, TileTypes::Hard );
	AddTile( 455, 300, TileTypes::Explosive );
	AddTile( 520, 300, TileTypes::Hard );
	AddTile( 585, 300, TileTypes::Unbreakable );

	AddTile( 325, 275, TileTypes::Unbreakable );
	AddTile( 390, 275, TileTypes::Hard );
	AddTile( 455, 275, TileTypes::Explosive );
	AddTile( 520, 275, TileTypes::Hard );
	AddTile( 585, 275, TileTypes::Unbreakable );

	AddTile( 325, 250, TileTypes::Unbreakable );
	AddTile( 390, 250, TileTypes::Hard );
	AddTile( 455, 250, TileTypes::Regular );
	AddTile( 520, 250, TileTypes::Hard );
	AddTile( 585, 250, TileTypes::Unbreakable );

	AddTile(   0, 225, TileTypes::Unbreakable );
	AddTile(  65, 225, TileTypes::Unbreakable );
	AddTile( 130, 225, TileTypes::Unbreakable );
	AddTile( 195, 225, TileTypes::Unbreakable );
	AddTile( 260, 225, TileTypes::Unbreakable );
	AddTile( 325, 225, TileTypes::Unbreakable );
	AddTile( 390, 225, TileTypes::Unbreakable );

	AddTile( 520, 225, TileTypes::Unbreakable );
	AddTile( 585, 225, TileTypes::Unbreakable );

	AddTile( 520, 200, TileTypes::Unbreakable );
	AddTile( 520, 175, TileTypes::Unbreakable );
	AddTile( 520, 150, TileTypes::Unbreakable );
	AddTile( 520, 125, TileTypes::Unbreakable );
	AddTile( 520, 100, TileTypes::Unbreakable );
	AddTile( 520,  75, TileTypes::Unbreakable );
	AddTile( 520,  50, TileTypes::Unbreakable );
	AddTile( 520,  25, TileTypes::Unbreakable );
	AddTile( 520,   0, TileTypes::Unbreakable );

	/*
	//================================ Original board code
	short x = 100;
	short y = 100;

	TileTypes outerLayer = TileTypes::Regular;

	AddTile( x, y, outerLayer);
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer);
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer);
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Explosive);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Explosive);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, outerLayer);
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, outerLayer );

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, outerLayer);
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive) ;
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;
	*/
}
