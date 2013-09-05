#include "GameManager.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"

#include <limits>
#include <iostream>

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
	localPaddle.reset( new Paddle() );
	localPaddle->textureType = GamePiece::Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.y = windowSize.h  - localPaddle->rect.h;
	renderer.SetLocalPaddle( localPaddle );

	std::cout << "y : " << localPaddle->rect.y << std::endl;

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

	std::shared_ptr< Ball > ball( new Ball() );
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
	std::shared_ptr< Tile > tile( new Tile( tileType, tileCount++  ) );
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

void GameManager::UpdateBalls( double delta )
{
	renderer.RenderLives( ballList.size(), Player::Local  );

	if ( ballList.size() > 0 )
	{
		renderer.RemoveText();

		for ( auto p = ballList.begin(); p != ballList.end() && (*p) != nullptr;  )
		{
			(*p)->Update( delta );
			(*p)->BoundCheck( windowSize );
			(*p)->PaddleCheck( localPaddle->rect );

			CheckBallTileIntersection( *p );

			if ( (*p)->DeathCheck( windowSize ) )
			{
				(*p)->rect.x = 200;
				(*p)->rect.y = 20;
				RemoveBall( (*p) );
				p = ballList.erase( p );
			}
			++p;
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

	while ( !quit )
	{
		bool delay1 = false;
		bool delay2 = false;
		bool delay3 = false;
		bool delay4 = false;

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

		if ( fpsLimit > 0 && delta < frameDuration )
		{
			unsigned short delay = static_cast< unsigned short > ( ( frameDuration  - (delta + 0.5 )  ) + 0.5 );

			if ( static_cast< unsigned short > ( delta ) < 60 )
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
	for ( auto p = tileList.begin(); p != tileList.end() && (*p) != nullptr;  )
	{
		tile = (*p);
		if ( ball->CheckTileSphereIntersection( tile->rect, ball->rect, current ) )
				//TileCheck( tile->rect, tile->GetTileID() ) )
		{
			if ( current  < closest )
			{
				closest = current;
				closestTile = tile;
				itClosestTile = p;
			}
						break;
		} else
		{
			++p;
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
			localPlayerPoints += points[ tile->GetTileTypeAsIndex() ];
			RemoveTile( tile );
			tileList.erase( itClosestTile );
		}
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
	short x = 100;
	short y = 100;

	TileTypes outerLayer = TileTypes::Unbreakable;

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
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, outerLayer);
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, outerLayer );

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer);
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, outerLayer );
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, outerLayer);
	x += 65;

	x = 100;
	y += 25;
	AddTile( x, y, outerLayer );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
	x += 65;
	AddTile( x, y, TileTypes::Explosive );
	x += 65;
	AddTile( x, y, TileTypes::Unbreakable );
	x += 65;
	AddTile( x, y, TileTypes::Hard );
	x += 65;
	AddTile( x, y, TileTypes::Regular);
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
}
