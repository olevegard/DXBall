#include "GameManager.h"

	GameManager::GameManager()
	:	renderer()
	,	timer()
	,	localPaddle()
	,	gameObjectList()
	,	paddleList()
	,	ballList()
	,	tileSize()
	,	windowSize()
{
}

int GameManager::Init()
{

	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
		return 1;

	if ( !renderer.Init() )
		return 1;

	tileSize = renderer.GetTileSize();
	windowSize = renderer.GetWindowSize();

	std::cout << "tile size " << tileSize.x << " , " << tileSize.y << " , " << tileSize.w << " , " << tileSize.h << std::endl;
	std::cout << "window size " << windowSize.x << " , " << windowSize.y << " , " << windowSize.w << " , " << windowSize.h << std::endl;

	Restart();

	return 0;
}

void GameManager::Restart()
{
	std::shared_ptr< GamePiece > paddle( new Paddle() );
	paddle->textureType = GamePiece::Paddle;
	paddle->rect.y = 610;
	paddle->rect.w = 120;
	paddle->rect.h = 30;

	renderer.AddObject( paddle );
	localPaddle = paddle.get();
	paddleList.push_back( dynamic_cast< Paddle* > ( paddle.get() ) );

	AddBullet();

	renderer.RenderLives( 1 );
	renderer.RenderPoints( 123 );
	renderer.RenderText( "Press enter to start");
}

void GameManager::AddBullet()
{
	std::shared_ptr< GamePiece > ball( new Ball() );
	ball->textureType = GamePiece::Ball;
	ballList.push_back( dynamic_cast<Ball *> ( ball.get() ) );
	gameObjectList.push_back( ball );

	renderer.AddObject( ball );
}

void GameManager::RemoveBall( const Ball* pBall )
{
	for ( auto p = gameObjectList.begin(); p != gameObjectList.end(); ) 
	{
		if ( ( *p ).get() == pBall )
		{
			renderer.RemoveObject( (*p ) );
			p = gameObjectList.erase( p );
			(*p).reset();
		}
		else
			++p;
	}
}

void GameManager::UpdateBalls( float delta )
{
	renderer.RenderLives( ballList.size() );

	if ( ballList.size() > 0 )
	{
		renderer.RemoveText();
		for ( std::vector< Ball* >::iterator p = ballList.begin(); p != ballList.end() && (*p) != NULL;  )
		{
			(*p)->Update( delta );
			(*p)->BoundCheck( windowSize );
			(*p)->PaddleCheck( localPaddle->rect );
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
		renderer.RenderText( "Press enter to start");
}
void GameManager::Run()
{
	bool quit = false;
	SDL_Event event;

	int halfTileWidth = tileSize.w / 2;

	while ( !quit )
	{
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
				quit = true;

			if ( event.type == SDL_KEYDOWN )
			{
				switch  ( event.key.keysym.sym )
				{
					case SDLK_RETURN:
					case SDLK_b:
						AddBullet();
						break;
					case SDLK_q:
					case SDLK_ESCAPE:
						quit = true;
						break;
					default:
						break;
				}
			}

			if ( event.motion.x != 0 && event.motion.y != 0 )
				localPaddle->rect.x = event.motion.x - halfTileWidth;

			if ( ( localPaddle->rect.x + tileSize.w) > windowSize.w )
				localPaddle->rect.x = windowSize.w - tileSize.w;

			if ( localPaddle->rect.x  <= 0  )
				localPaddle->rect.x = 0;
		}
		float delta = timer.GetDelta( );
		UpdateBalls( delta );

		renderer.Render( );
	}

}
