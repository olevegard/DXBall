#pragma once

#include <iostream>

#include "Timer.h"
#include "Ball.h"
#include "Paddle.h"
#include "GamePiece.h"
#include "Renderer.h"

class GameManager
{
	public:
		GameManager();

		~GameManager()
		{

		}
		//GameManager( const GameManager &gm );
		//GameManager operator=( const GameManager &gm );

		int Init();

		void Restart();

		void AddBullet();
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void UpdateBalls( double delta );

		void Run();
	private:
		Renderer renderer;
		Timer timer;

		std::shared_ptr < Paddle > localPaddle;
		std::vector< std::shared_ptr< Ball > > ballList;

		SDL_Rect tileSize;
		SDL_Rect windowSize;
};
