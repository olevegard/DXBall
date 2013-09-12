#pragma once

#include "Timer.h"
#include "Renderer.h"

#include <vector>

// Forward declarations
enum class TileTypes : int;
enum class Player : int;
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

		void AddBall( Player owner );
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void AddTile( short posX, short posY, TileTypes tileType );
		void RemoveTile( std::shared_ptr< Tile > pTile );

		void UpdateBalls( double delta );
		void UpdateGUI( );

		void Run();

		void SetTwoPlayerMode( bool isTwoPlayer )
		{
			isTwoPlayerMode = isTwoPlayer;
		}


		void SetFPSLimit( unsigned short limit );
	private:

		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );

		void HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner  );
		std::vector< std::shared_ptr< Tile > > FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const;
		std::vector< Rect > GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const;

		void GenerateBoard();
		void IncrementPoints( size_t tileType, bool isDestroyed, Player ballOwner );

		Renderer renderer;
		Timer timer;

		std::shared_ptr < Paddle > localPaddle;
		std::shared_ptr < Paddle > remotePaddle;

		bool isTwoPlayerMode;
		unsigned int localPlayerPoints;
		unsigned int localPlayerLives;
		unsigned int localPlayerActiveBalls;

		unsigned int remotePlayerPoints;
		unsigned int remotePlayerLives;
		unsigned int remotePlayerActiveBalls;

		std::vector< std::shared_ptr< Ball > > ballList;
		std::vector< std::shared_ptr< Tile > > tileList;

		SDL_Rect windowSize;
		unsigned int points[4];

		unsigned int tileCount;

		unsigned short fpsLimit;
		double frameDuration;

};
