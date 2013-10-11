#pragma once

#include "Timer.h"
#include "Renderer.h"
#include "NetManager.h"
#include "BoardLoader.h"
#include "MenuManager.h"

#include <vector>

// Forward declarations
class TCPMessage;
enum class TileType : int;
enum class Player : int;
struct GamePiece;
struct Paddle;
struct Ball;
struct Tile;
class GameManager
{
	public:
		GameManager();

		bool Init( const std::string &localPlayerName, const std::string &remotePlayerName, const SDL_Rect &size, bool startFS );

		void SetSize( const SDL_Rect &size );

		void Restart();

		std::shared_ptr<Ball> AddBall( Player owner, unsigned int ballID );
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void AddTile( short posX, short posY, TileType tileType );
		void RemoveTile( std::shared_ptr< Tile > pTile );

		void AddBonusBox(const std::shared_ptr< Ball > &triggerBall, double x, double y, int tilesDestroyed = 1 );
		void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );

		void UpdateBalls( double delta );
		void UpdateTileHit( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile );
		void DeleteDeadBalls();

		void UpdateGUI( );

		void Run();
		void AIMove();
		void AIMove_Local();

		void CreateMenu();

		void SetFPSLimit( unsigned short limit );
		void SetTwoPlayerMode( bool isTwoPlayer )
		{
			isTwoPlayerMode = isTwoPlayer;
		}

		void InitNetManager( bool isServer, std::string ip, unsigned short port );
	private:
		// Tile collisions
		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );
		std::shared_ptr< Tile > FindClosestIntersectingTile( std::shared_ptr< Ball > ball );
		void RemoveClosestTile(std::shared_ptr< Ball > ball, std::shared_ptr< Tile > closestTile );

		// Explotion related
		int HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner  );
		std::vector< std::shared_ptr< Tile > > FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const;
		std::vector< Rect > GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const;
		void RemoveDeadTiles();
	
		// Board handling
		void GenerateBoard();
		void ClearBoard();
		bool IsLevelDone();

		void IncrementPoints( size_t tileType, bool isDestroyed, Player ballOwner );

		void Update( double delta );

		// Bonus boxes
		void UpdateBonusBoxes( double delta );
		void MoveBonusBoxes( double delta );
		void RemoveDeadBonusBoxes();
		
		// Network
		void PrintSend( const TCPMessage &msg ) const;
		void PrintRecv( const TCPMessage &msg ) const;

		void UpdateNetwork();
		void SendBallSpawnMessage( const std::shared_ptr<Ball> &ball);
		void SendBallDataMessage( const std::shared_ptr<Ball> &ball);
		void SendBallKilledMessage( const std::shared_ptr<Ball> &ball);

		void SendTileHitMessage( unsigned int tileID );

		void SendPaddlePosMessage( );

		void SetLocalPaddlePosition( int x, int y );
		void HandleMouseEvent(  const SDL_MouseButtonEvent &buttonEvent );

		std::shared_ptr< Ball > GetBallFromID( unsigned int ID );
		std::shared_ptr< Tile > GetTileFromID( unsigned int ID );

		void DoFPSDelay( unsigned int ticks );

		BoardLoader boardLoader;
		Renderer renderer;
		Timer timer;
		MenuManager menuManager;
		NetManager netManager;

		std::shared_ptr < Paddle > localPaddle;
		std::shared_ptr < Paddle > remotePaddle;

		bool isTwoPlayerMode;
		unsigned int localPlayerPoints;
		unsigned int localPlayerLives;
		unsigned int localPlayerActiveBalls;

		unsigned int remotePlayerPoints;
		unsigned int remotePlayerLives;
		unsigned int remotePlayerActiveBalls;

		std::vector< std::shared_ptr< Ball     > > ballList;
		std::vector< std::shared_ptr< Tile     > > tileList;
		std::vector< std::shared_ptr< BonusBox > > bonusBoxList;

		SDL_Rect windowSize;
		unsigned int points[4];

		unsigned int tileCount;
		unsigned int ballCount;

		unsigned short fpsLimit;
		double frameDuration;
};
