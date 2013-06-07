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

		GameManager( const GameManager &gm );
		GameManager operator=( const GameManager &gm );

		int Init();

		void Restart();

		void AddBullet();
		void RemoveBall( const Ball* pBall );

		void UpdateBalls( float delta );

		void Run();
	private:
		Renderer renderer;
		Timer timer;
		GamePiece* localPaddle;

		std::vector< std::shared_ptr< GamePiece > > gameObjectList;
		std::vector< Paddle* > paddleList;
		std::vector< Ball* > ballList;

		SDL_Rect tileSize;
		SDL_Rect windowSize;
};
