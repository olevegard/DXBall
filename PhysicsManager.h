#pragma once

#include <vector>
#include <memory>

#include "MessageSender.h"

#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Bullet.h"
#include "structs/game_objects/Paddle.h"
#include "structs/game_objects/BonusBox.h"

class PhysicsManager
{
public:

	PhysicsManager( MessageSender &msgSender, Logger &logger_ );

	// Tiles
	// =============================================================================================================
	void AddTile( const std::shared_ptr< Tile > &tile );
	void RemoveTile( const std::shared_ptr< Tile >  &tile );
	std::shared_ptr< Tile > CreateTile( const Vector2f &pos, const TileType &tileType, int32_t tileID = -1 );
	void RemoveTileWithID( int32_t ID );

	std::shared_ptr< Tile > GetTileWithID( int32_t ID);

	std::shared_ptr< Tile > FindClosestIntersectingTile( std::shared_ptr< Ball > ball );
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

	bool DidBulletHitTile( std::shared_ptr< Bullet > bullet, std::shared_ptr< Tile > tile );
	std::shared_ptr< Tile > CheckBulletTileIntersections( std::shared_ptr< Bullet > bullet );

	// Bonus Boxes
	// =============================================================================================================
	void AddBonusBox( const std::shared_ptr< BonusBox > &bb );
	void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );
	std::shared_ptr< BonusBox > CreateBonusBox( uint32_t ID, const Player &owner, const Vector2f &dir, const Vector2f &pos );

	std::shared_ptr< BonusBox > GetBonusBoxWithID( int32_t ID, const Player &owner   );

	void MoveBonusBoxes( double delta );
	void SetBonusBoxDirection( std::shared_ptr< BonusBox > bonusBox, Vector2f dir_ ) const;
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
	void SetPaddles( std::shared_ptr < Paddle > localPaddle_, std::shared_ptr < Paddle > remotePaddle_ );

	void Clear();

	void UpdateScale();
private:
	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
	std::vector< std::shared_ptr< Bullet > > bulletList;

	std::shared_ptr < Paddle > localPaddle;
	std::shared_ptr < Paddle > remotePaddle;

	MessageSender 	&messageSender;
	Logger			&logger;

	SDL_Rect windowSize;
	double scale;

	double bulletSpeed;
	double bonusBoxSpeed;

	uint32_t objectCount;
};
