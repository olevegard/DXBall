#pragma once

#include "Timer.h"
#include "Logger.h"
#include "Renderer.h"
#include "NetManager.h"
#include "BoardLoader.h"
#include "MenuManager.h"
#include "ConfigLoader.h"
#include "MessageSender.h"
#include "PhysicsManager.h"

#include "structs/PlayerInfo.h"

#include <vector>


enum class DirectionX{ Left, Middle, Right };

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

		void InitNetManager( std::string ip_, uint16_t port_ );
	private:
		// Bonus Boxes
		// ===========================================
		bool WasBonusBoxSpawned( int32_t tilesDestroyed ) const;
		BonusType GetRandomBonusType() const;

		void AddBonusBox(const std::shared_ptr< Ball > &triggerBall, double x, double y, int tilesDestroyed = 1 );
		void AddBonusBox( const Player &owner, Vector2f dir,  const Vector2f &pos, int tilesDestroyed = 1 );
		void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );
		void DeleteAllBonusBoxes();
		void RemoveDeadBonusBoxes();

		std::shared_ptr< BonusBox > GetBonusBoxFromID( int32_t ID );
		void ApplyBonus( std::shared_ptr< BonusBox > &ptr );
		void ApplyBonus_Death( const Player &player );

		// Ball
		// ===========================================
		void UpdateTileHit( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile );
		void UpdateBallSpeed();
		void IncreaseActiveBalls( const Player &player );
		void ReduceActiveBalls( const Player &player, uint32_t ballID );
		void CheckBallSpeedFastMode( double delta);
		void IncreaseBallSpeedFastMode( const Player &player, double delta );

		void DeleteDeadBalls();
		void DeleteDeadTiles();
		std::shared_ptr<Ball> AddBall( );
		std::shared_ptr<Ball> AddBall( Player owner, unsigned int ballID );
		void RemoveBall( std::shared_ptr< Ball > pBall );

		bool IsSuperBall( std::shared_ptr< Ball > ball );
		double GetBallSpeed( const Player &player ) const;
		bool CanPlayerFireBall( const Player &player ) const;

		// Bullets
		// ===========================================
		void CheckBulletTileIntersections( std::shared_ptr< Bullet > bullet );
		void HandleBulletTileIntersection( std::shared_ptr< Bullet > bullet, std::shared_ptr< Tile > tile );

		void DeleteDeadBullets();
		void DeleteAllBullets();
		void FireBullets();
		std::shared_ptr< Bullet >  FireBullet( int32_t id, const Player &owner, Vector2f pos );

		bool IsSuperBullet( const Player owner ) const;

		// Tiles
		// ==========================================
		void AddTile( const Vector2f &pos, TileType tileType );

		void RemoveTile( std::shared_ptr< Tile > pTile );

		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );
		void RemoveClosestTile(std::shared_ptr< Ball > ball, std::shared_ptr< Tile > closestTile );

		void RecieveTileSpawnMessage( const TCPMessage &message );
		// Paddles
		// ===========================================
		void SetLocalPaddlePosition( int x, int y );

		// Config
		// ===========================================
		void LoadConfig();

		// Joystick
		// ==========================================
		DirectionX GetJoystickDirection( int32_t axis );
		void DoJoystickMovement( const DirectionX &dirX );

		// Game status
		// ==========================================
		void HandleStatusChange( );
		void Restart();
		void CheckIfGameIsOver();

		// AI
		// ==========================================
		void AIMove();
		bool IsTimeForAIMove( std::shared_ptr< Ball > highest ) const;

		void CreateMenu();

		// New Game / Join Game / Update
		// ==========================================
		void StartNewGame();
		void JoinGame();

		// Explotion related
		// ===========================================
		int HandleExplosions( const std::shared_ptr< Tile > &explodingTile, Player ballOwner  );

		// Board handling
		// ===========================================
		void GenerateBoard();
		void ClearBoard();
		bool IsLevelDone();

		// Points / Lives
		// ===========================================
		void IncrementPoints( const TileType &tileType, bool isDestroyed, Player ballOwner );
		void ReducePlayerLifes( Player player );

		// Update
		// ===========================================
		void Update( double delta );
		void UpdateGameObjects( double delta );
		void UpdateBonusBoxes( double delta );
		void UpdateBullets( double delta );
		void UpdateBalls( double delta );

		void UpdateLobbyState();
		void UpdateJoystick( );
		void UpdateGameList();
		void UpdateNetwork();
		void UpdateBoard();
		void UpdateGUI( );

		// Input
		// ===========================================
		void HandleEvent( const SDL_Event &event );
		void HandleMouseEvent(  const SDL_MouseButtonEvent &buttonEvent );
		void HandleKeyboardEvent( const SDL_Event &event );
		void HandleMenuKeys( const SDL_Event &event );
		void HandleGameKeys( const SDL_Event &event );
		void HandleJoystickEvent( const SDL_JoyButtonEvent &event );

		// Input
		// ===========================================
		void InitJoystick();
		void InitPaddles();
		void InitMenu();


		// Network
		// ===========================================
		void PrintSend( const TCPMessage &msg ) const;
		void PrintRecv( const TCPMessage &msg ) const;


		// Recieve messages
		// ===========================================
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
		void RecieveBulletFireMessage( const TCPMessage &message );
		void RecieveBulletKillMessage( const TCPMessage &message );

		void DoFPSDelay( unsigned int ticks );

		// Rendering
		// ===========================================
		void RendererScores();
		void RenderInGame();
		void RenderEndGame();

		// Variables
		// ===========================================
		PlayerInfo localPlayerInfo;
		PlayerInfo remotePlayerInfo;

		BoardLoader boardLoader;
		Renderer renderer;
		Timer timer;
		MenuManager menuManager;
		ConfigLoader gameConfig;
		NetManager netManager;
		Logger logger;
		MessageSender messageSender;
		PhysicsManager physicsManager;

		bool runGame;

		std::string ip;
		uint16_t port;
		int32_t gameID;

		std::shared_ptr < Paddle > localPaddle;
		std::shared_ptr < Paddle > remotePaddle;

		bool isAIControlled;

		std::vector< std::shared_ptr< Ball     > > ballList;
		std::vector< std::shared_ptr< Tile     > > tileList;
		std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
		std::vector< std::shared_ptr< Bullet   > > bulletList;

		SDL_Rect windowSize;
		double remoteResolutionScale;

		unsigned short fpsLimit;
		double frameDuration;

		SDL_Joystick *stick;
};
