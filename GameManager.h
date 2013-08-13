#pragma once

#include <iostream>

#include "Timer.h"
#include "Ball.h"
#include "Tile.h"
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

		void AddBall();
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void AddTile();
		void RemoveTile( std::shared_ptr< Ball > pTile );

		void UpdateBalls( double delta );

		void Run();

		void CheckBallTileIntersection();
	private:
		Renderer renderer;
		Timer timer;

		std::shared_ptr < Paddle > localPaddle;
		std::vector< std::shared_ptr< Ball > > ballList;
		std::vector< std::shared_ptr< Tile > > tileList;

		SDL_Rect tileSize;
		SDL_Rect windowSize;
};
