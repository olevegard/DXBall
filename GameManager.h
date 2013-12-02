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
enum class MessageTarget : int;
struct GamePiece;
struct Bullet;
struct Paddle;
struct Ball;
struct Tile;
class GameManager
{
	public:
		GameManager();
		// Startup options
		bool Init( const std::string &localPlayerName, const SDL_Rect &size, bool startFS );

		// Setters
		void SetFPSLimit( unsigned short limit );
		void SetAIControlled( bool isAIControlled_ );

		// Ipdate
		void Run();
		void UpdateGUI( );

		void InitNetManager( std::string ip_, uint16_t port_ );
	private:
		// Add / Remoe game objects
		std::shared_ptr<Ball> AddBall( Player owner, unsigned int ballID );
		void RemoveBall( std::shared_ptr< Ball > pBall );

		void AddTile( short posX, short posY, TileType tileType );
		void RemoveTile( std::shared_ptr< Tile > pTile );

		void AddBonusBox(const std::shared_ptr< Ball > &triggerBall, double x, double y, int tilesDestroyed = 1 );
		void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );

		// Ball checks
		void UpdateBalls( double delta );
		void UpdateBullets( double delta );
		void UpdateTileHit( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile );
		void DeleteDeadBalls();
		void DeleteDeadTiles();
		void DeleteDeadBullets();

		// Input
		void HandleEvent( const SDL_Event &event );
		void HandleMouseEvent(  const SDL_MouseButtonEvent &buttonEvent );
		void HandleKeyboardEvent( const SDL_Event &event );
		void HandleMenuKeys( const SDL_Event &event );
		void HandleGameKeys( const SDL_Event &event );

		// Game status
		void HandleStatusChange( );
		void Restart();

		// AI
		void AIMove();
		std::shared_ptr< Ball > FindHighestBall();
		bool IsTimeForAIMove( std::shared_ptr< Ball > highest ) const;

		void CreateMenu();

		// New Game / Join Game / Update
		void StartNewGame();
		void GoBackFromLobby();
		void JoinGame();

		// Tile collisions
		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );
		std::shared_ptr< Tile > FindClosestIntersectingTile( std::shared_ptr< Ball > ball );
		void RemoveClosestTile(std::shared_ptr< Ball > ball, std::shared_ptr< Tile > closestTile );
		bool IsSuperBall( std::shared_ptr< Ball > ball );

		// Explotion related
		int HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner  );
		std::vector< std::shared_ptr< Tile > > FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const;
		std::vector< Rect > GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const;

		// Board handling
		void GenerateBoard();
		void ClearBoard();
		bool IsLevelDone();
		int32_t CountDestroyableTiles();
		bool IsTimeForNewBoard();

		void IncrementPoints( size_t tileType, bool isDestroyed, Player ballOwner );
		void ReducePlayerLifes( Player player );
		void RemoveDeadBallsAndBoxes( Player player );

		void Update( double delta );
		void UpdateLobbyState();
		void UpdateGameList();

		// Bonus boxes
		void UpdateBonusBoxes( double delta );
		void MoveBonusBoxes( double delta );
		void RemoveDeadBonusBoxes();
		void ApplyBonus( std::shared_ptr< BonusBox > &ptr );
		BonusType GetRandomBonusType() const;

		// Network
		void PrintSend( const TCPMessage &msg ) const;
		void PrintRecv( const TCPMessage &msg ) const;

		void UpdateNetwork();

		// Recieve messages
		void ReadMessages( );
		void ReadMessagesFromServer( );
		void HandleRecieveMessage( const TCPMessage &message );

		void RecieveJoinGameMessage( const TCPMessage &message );
		void RecieveNewGameMessage( const TCPMessage &message );
		void RecieveEndGameMessage( const TCPMessage &message );
		void RecievePlayerNameMessage( const TCPMessage &message );
		void RecieveGameSettingsMessage( const TCPMessage &message);
		void RecieveGameStateChangedMessage( const TCPMessage &message );
		void RecieveBallSpawnMessage( const TCPMessage &message );
		void RecieveBallDataMessage( const TCPMessage &message );
		void RecieveBallKillMessage( const TCPMessage &message );
		void RecieveTileHitMessage( const TCPMessage &message );
		void RecievePaddlePosMessage( const TCPMessage &message );
		void RecieveBonusBoxSpawnedMessage( const TCPMessage &message );
		void RecieveBonusBoxPickupMessage( const TCPMessage &message );

		// Send messages
		void SendPlayerName();
		void SendNewGameMessage( );
		void SendJoinGameMessage(const GameInfo &gameInfo );
		void SendEndGameMessage( );
		void SendGetGameListMessage();
		void SendGameSettingsMessage();
		void SendGameStateChangedMessage( );
		void SendBallSpawnMessage( const std::shared_ptr<Ball> &ball);
		void SendBallDataMessage( const std::shared_ptr<Ball> &ball);
		void SendBallKilledMessage( const std::shared_ptr<Ball> &ball);
		void SendTileHitMessage( unsigned int tileID );
		void SendPaddlePosMessage( );
		void SendBonusBoxSpawnedMessage( const std::shared_ptr< BonusBox > &bonusBox );
		void SendBonusBoxPickupMessage( const std::shared_ptr< BonusBox > &bonusBox );
		void SendMessage( const TCPMessage &message, const MessageTarget &target );

		std::shared_ptr< Ball > GetBallFromID( int32_t ID );
		std::shared_ptr< Tile > GetTileFromID( int32_t ID );
		std::shared_ptr< BonusBox > GetBonusBoxFromID( int32_t ID );

		// Paddles
		void SetLocalPaddlePosition( int x, int y );

		void DoFPSDelay( unsigned int ticks );

		// Scaling
		void SetScale( double scale );
		void ApplyScale( );
		void ResetScale( );

		// Rendering
		void RendererScores();
		void RenderInGame();
		void RenderEndGame();

		// Variables
		bool runGame;
		BoardLoader boardLoader;
		Renderer renderer;
		Timer timer;
		MenuManager menuManager;

		NetManager netManager;
		std::string ip;
		uint16_t port;
		int32_t gameID;

		std::shared_ptr < Paddle > localPaddle;
		std::shared_ptr < Paddle > remotePaddle;

		bool isAIControlled;
		std::string localPlayerName;
		unsigned int localPlayerPoints;
		unsigned int localPlayerLives;
		unsigned int localPlayerActiveBalls;
		bool localPlayerSuperBall;

		std::string remotePlayerName;
		unsigned int remotePlayerPoints;
		unsigned int remotePlayerLives;
		unsigned int remotePlayerActiveBalls;
		bool remotePlayerSuperBall;

		std::vector< std::shared_ptr< Ball     > > ballList;
		std::vector< std::shared_ptr< Tile     > > tileList;
		std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
		std::vector< std::shared_ptr< Bullet   > > bulletList;

		SDL_Rect windowSize;
		double scale;
		unsigned int points[4];

		double remoteResolutionScale;
		bool isResolutionScaleRecieved;

		unsigned int tileCount;
		unsigned int ballCount;
		unsigned int bonusCount;

		unsigned short fpsLimit;
		double frameDuration;

		double ballSpeed;
		double bonusBoxSpeed;
};
