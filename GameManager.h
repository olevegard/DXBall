#pragma once

#include "Timer.h"
#include "Renderer.h"

#include <vector>

#include <SDL2/SDL.h>

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
		bool Init( const std::string &localPlayerName, const std::string &remotePlayerName, const SDL_Rect &size, bool startFS );

		void SetSize( const SDL_Rect &size );

		void Restart();

		void AddBall();
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void AddTile( short posX, short posY, TileTypes tileType );
		void RemoveTile( std::shared_ptr< Tile > pTile );

		void UpdateBalls( double delta );
		void UpdateGUI( );

		void Run();


		void SetFPSLimit( unsigned short limit );
	private:

		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );

		void HandleExplosions( const std::shared_ptr< Tile > &explodingTile );
		std::vector< std::shared_ptr< Tile > > FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const;
		std::vector< Rect > GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const;

		void GenerateBoard();
		void IncrementPoints( size_t tileType, bool isDestroyed );

		Renderer renderer;
		Timer timer;

		std::shared_ptr < Paddle > localPaddle;


		unsigned int localPlayerPoints;
		unsigned int localPlayerLives;
		unsigned int localPlayerActiveBalls;

		std::vector< std::shared_ptr< Ball > > ballList;
		std::vector< std::shared_ptr< Tile > > tileList;

		SDL_Rect windowSize;
		unsigned int points[4];

		unsigned int tileCount;

		unsigned short fpsLimit;
		double frameDuration;

};
