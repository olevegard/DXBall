#pragma once

#include <iostream>

#include "Timer.h"
#include "Renderer.h"

// Forward declarations
enum class TileTypes : int;
struct GamePiece;
struct Paddle;
struct Ball;
struct Tile;

class GameManager
{
	public:
		GameManager();

		~GameManager()
		{

		}
		int Init();

		void Restart();

		void AddBall();
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void AddTile( short posX, short posY, TileTypes tileType );
		void RemoveTile( std::shared_ptr< Tile > pTile );

		void UpdateBalls( double delta );
		void UpdateGUI( );

		void Run();

		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );
	private:
		Renderer renderer;
		Timer timer;

		std::shared_ptr < Paddle > localPaddle;

		unsigned int localPlayerPoints;
		unsigned int localPlayerLives;
		unsigned int localPlayerActiveBalls;

		std::vector< std::shared_ptr< Ball > > ballList;
		std::vector< std::shared_ptr< Tile > > tileList;

		SDL_Rect tileSize;
		SDL_Rect windowSize;

		unsigned int points[4];
};
