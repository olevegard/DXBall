#pragma once

#include <vector>
#include <memory>

#include "math/Vector2f.h"
#include "math/Rect.h"
#include "enums/TileType.h"

#include <SDL2/SDL.h>

struct GamePiece;
struct BonusBox;
struct Bullet;
struct Paddle;
struct Ball;
struct Tile;
enum class Player : int;
enum class BonusType : int;
class Logger;
class MessageSender;

class PhysicsManager
{
public:

	PhysicsManager( MessageSender &msgSender );

	// Tiles
	// =============================================================================================================
	void AddTile( const std::shared_ptr< Tile > &tile );
	void RemoveTile( const std::shared_ptr< Tile >  &tile );
	std::shared_ptr< Tile > CreateTile( const Vector2f &pos, const TileType &tileType, int32_t tileID = -1 );
	void RemoveTileWithID( int32_t ID );

	std::shared_ptr< Tile > GetTileWithID( int32_t ID);

	std::shared_ptr< Tile > FindClosestIntersectingTile( const std::shared_ptr< Ball > &ball );
	bool KillAllTilesWithOwner( const Player &player );

	int32_t CountDestroyableTiles();
	int32_t CountAllTiles();
	void PrintTileList() const;

	// Balls
	// =============================================================================================================
	void AddBall( const std::shared_ptr< Ball > &ball );
	std::shared_ptr< Ball >  CreateBall( const Player &player, uint32_t ballID, double speed );
	void RemoveBall( const std::shared_ptr< Ball >  &ball );
	void RemoveBallWithID( int32_t ID, const Player &owner );

	std::shared_ptr< Ball > GetBallWithID( int32_t ID, const Player &owner );
	std::shared_ptr< Ball > FindHighestBall();

	void UpdateBallSpeed( double localPlayerSpeed, double remotePlayerSpeed );

	bool DidBulletHitTile( const std::shared_ptr< Bullet > &bullet, const std::shared_ptr< Tile > &tile );
	std::shared_ptr< Tile > CheckBulletTileIntersections( const std::shared_ptr< Bullet > &bullet );

	std::vector< std::shared_ptr< Tile > >  FindAllTilesOnBulletsPath( const std::shared_ptr< Bullet > &bullet );

	void RespawnBalls( const Player &owner, double ballSpeed );

	// Bonus Boxes
	// =============================================================================================================
	void AddBonusBox( const std::shared_ptr< BonusBox > &bb );
	void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );
	std::shared_ptr< BonusBox > CreateBonusBox( uint32_t ID, const Player &owner, const Vector2f &dir, const Vector2f &pos );

	std::shared_ptr< BonusBox > GetBonusBoxWithID( int32_t ID, const Player &owner   );

	void MoveBonusBoxes( double delta );
	void SetBonusBoxDirection( const std::shared_ptr< BonusBox > &bonusBox, Vector2f dir_ ) const;
	BonusType GetRandomBonusType() const;

	// Bullets
	// =============================================================================================================
	void AddBullet( const std::shared_ptr< Bullet > &bullet );
	void RemoveBullet( const std::shared_ptr< Bullet >  &bullet );
	std::shared_ptr< Bullet >  CreateBullet( int32_t id, const Player &owner, Vector2f pos );

	std::shared_ptr< Bullet > GetBulletWithID( int32_t ID, const Player &owner  );

	void KillBulletWithID( int32_t id, const Player &owner );

	// Paddles
	// =============================================================================================================
	void SetPaddleData( );
	void SetLocalPaddlePosition( int32_t x );
	bool IsTimeForAIMove( std::shared_ptr< Ball > highest ) const;
	void AIMove();

	// Explosions
	// =============================================================================================================
	std::vector< Rect > GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const;
	std::vector< std::shared_ptr< Tile > > FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const;

	double ResetScale( );
	void ApplyScale( double scale_ );
	void KillBallsAndBonusBoxes( const Player &player );

	void SetScale( double scale_ );
	void SetBulletSpeed( double bulletSpeed_ );
	void SetBonusBoxSpeed( double bonusBoxSpeed_ );
	void SetWindowSize( const SDL_Rect &wSize );
	void SetPaddles( const std::shared_ptr < Paddle > &localPaddle_, const std::shared_ptr < Paddle > &remotePaddle_ );

	void Clear();

	void UpdateScale();
	double GetScale() const;
private:
	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
	std::vector< std::shared_ptr< Bullet > > bulletList;

	std::shared_ptr < Paddle > localPaddle;
	std::shared_ptr < Paddle > remotePaddle;

	MessageSender 	&messageSender;
	Logger			*logger;

	SDL_Rect windowSize;
	double scale;

	double bulletSpeed;
	double bonusBoxSpeed;

	uint32_t objectCount;
};
