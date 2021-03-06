#include "PhysicsManager.h"

#include <algorithm>
#include <csignal>

#include "math/RectHelpers.h"
#include "math/Math.h"

#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Paddle.h"
#include "structs/game_objects/Bullet.h"
#include "structs/game_objects/BonusBox.h"

#include "Logger.h"
#include "MessageSender.h"

PhysicsManager::PhysicsManager( MessageSender &msgSender )
	:	messageSender( msgSender )
	,	scale( 1.0 )
	,	objectCount ( 0 )
{
	logger = Logger::Instance();
}
void PhysicsManager::AddTile( const std::shared_ptr< Tile > &tile )
{
	tileList.push_back( tile );
}
void PhysicsManager::RemoveTile( const std::shared_ptr< Tile >  &tile )
{
	tileList.erase( std::find( tileList.begin(), tileList.end(), tile) );
}
std::shared_ptr< Tile > PhysicsManager::GetTileWithID( int32_t ID)
{
	for ( const auto &p : tileList )
	{
		if ( ID == p->GetObjectID() )
			return p;
	}

	logger->Log( __FILE__, __LINE__, "Tile doesn't exist : ", ID );
	raise ( SIGABRT );

	return nullptr;
}
std::shared_ptr< Tile > PhysicsManager::PhysicsManager::CreateTile( const Vector2f &pos, const TileType &tileType, int32_t tileID )
{
	if ( tileID == -1 )
		tileID = ++objectCount;

	std::shared_ptr< Tile > tile = std::make_shared< Tile >( tileType, tileID );
	tile->textureType = TextureType::e_Tile;

	tile->rect.x = static_cast< int32_t > ( pos.x );
	tile->rect.y = static_cast< int32_t > ( pos.y );
	tile->rect.w = 60 * scale;
	tile->rect.h = 20 * scale;

	AddTile( tile );

	return tile;
}
std::shared_ptr< Tile > PhysicsManager::FindClosestIntersectingTile( const std::shared_ptr< Ball > &ball )
{
	std::shared_ptr< Tile > closestTile;
	double closest = std::numeric_limits< double >::max();
	double current = closest;

	for ( const auto &p : tileList)
	{
		if ( !p->IsAlive() )
			continue;

		if ( !ball->CheckTileSphereIntersection( p->rect, ball->rect, current ) )
		{
			current = std::numeric_limits< double >::max();
			continue;
		}

		if ( current < closest )
		{
			closest = current;
			closestTile = p;
		}
	}

	return closestTile;
}
int32_t PhysicsManager::CountDestroyableTiles()
{
	auto IsTileDestroyable = []( const std::shared_ptr< Tile > &tile )
	{
		return ( tile->GetTileType() != TileType::Unbreakable );
	};

	return static_cast< int32_t > ( std::count_if( tileList.begin(), tileList.end(), IsTileDestroyable ) );
}
int32_t PhysicsManager::CountAllTiles()
{
	return static_cast< int32_t> ( tileList.size() );
}
void PhysicsManager::PrintTileList() const
{
	logger->Log( __FILE__, __LINE__, "==================== Tile List  ====================");
	for ( const auto &tile : tileList )
	{
		logger->Log( __FILE__, __LINE__, "========================New Tile", tile->GetObjectID() );
		logger->Log( __FILE__, __LINE__, "Tile Type", tile->GetTileTypeAsIndex() );
		logger->Log( __FILE__, __LINE__, "Tile pos", tile->GetPosition() );
		logger->Log( __FILE__, __LINE__, "================================", tile->GetObjectID() );
	}
	logger->Log( __FILE__, __LINE__, "=============================== ====================");
}
// Balls
// =============================================================================================================
void PhysicsManager::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}
void PhysicsManager::RemoveBall( const std::shared_ptr< Ball >  &ball )
{
	ballList.erase( std::find( ballList.begin(), ballList.end(), ball) );
}
std::shared_ptr< Ball >  PhysicsManager::CreateBall( const Player &owner, uint32_t ballID, double speed )
{
	if ( owner == Player::Local )
		ballID = ++objectCount;

	std::shared_ptr< Ball > ball = std::make_shared< Ball >( windowSize, owner, ballID );
	ball->textureType = TextureType::e_Ball;

	double scale_ = ( windowSize.h ) / 1080.0;
	ball->SetSpeed( speed * scale_ );

	ballList.push_back( ball );

	return ball;
}
void PhysicsManager::RemoveBallWithID( int32_t ID, const Player &owner )
{
	const auto &ball =  GetBallWithID(  ID, owner );

	if ( ball != nullptr )
		ball->Kill();
}
std::shared_ptr< Ball > PhysicsManager::GetBallWithID( int32_t ID, const Player &owner )
{
	for ( const auto &p : ballList )
	{
		if ( ID == p->GetObjectID() && p->GetOwner() == owner  )
		{
			return p;
		}
	}

	logger->Log( __FILE__, __LINE__, "Ball doesn't exist : ", ID );
	raise ( SIGABRT );

	return nullptr;
}
bool PhysicsManager::KillAllBallsWithOwner( const Player &player )
{
	bool tilesKilled = false;
	for ( const auto &p : ballList )
	{
		if ( p->GetOwner() == player )
		{
			p->Kill();
			tilesKilled = true;
		}
	}
	return tilesKilled;
}
void PhysicsManager::UpdateBallSpeed( double localPlayerSpeed, double remotePlayerSpeed )
{
	auto setBallSpeed = [=]( std::shared_ptr< Ball > curr )
	{
		if ( curr->GetOwner() == Player::Local )
			curr->SetSpeed( localPlayerSpeed );
		else
			curr->SetSpeed( remotePlayerSpeed );
	};
	std::for_each( ballList.begin(), ballList.end(), setBallSpeed );
}
std::shared_ptr< Ball > PhysicsManager::FindHighestBall()
{
	double yMax = 0;
	std::shared_ptr< Ball > highest = nullptr;;
	for ( const auto &p : ballList )
	{
		if ( p->GetOwner() == Player::Local )
		{
			if ( p->rect.y > yMax )
			{
				highest = p;
				yMax = p->rect.y;
			}
		}
	}
	return highest;
}
// Bonus Boxes
// =============================================================================================================
void PhysicsManager::AddBonusBox( const std::shared_ptr< BonusBox > &bb )
{
	bonusBoxList.push_back( bb );
}
void PhysicsManager::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
{
	bonusBoxList.erase( std::find( bonusBoxList.begin(), bonusBoxList.end(), bb) );
}
std::shared_ptr< BonusBox > PhysicsManager::CreateBonusBox( uint32_t ID, const Player &owner, const Vector2f &dir, const Vector2f &pos )
{
	std::shared_ptr< BonusBox > bonusBox = std::make_shared< BonusBox >( ID );

	if ( owner == Player::Local )
	{
		bonusBox->SetObjectID( ++objectCount );
		bonusBox->SetBonusType( GetRandomBonusType() );

		bonusBox->SetPosition( pos );
	}
	bonusBox->SetOwner( owner  );
	bonusBox->SetSpeed( bonusBoxSpeed );
	SetBonusBoxDirection( bonusBox, dir );

	bonusBoxList.push_back( bonusBox );
	return bonusBox;
}
std::shared_ptr< BonusBox > PhysicsManager::GetBonusBoxWithID( int32_t ID, const Player &owner )
{
	for ( const auto &p : bonusBoxList )
	{
		if ( ID == p->GetObjectID() && owner == p->GetOwner() )
		{
			return p;
		}
	}

	logger->Log( __FILE__, __LINE__, "BonusBox doesn't exist : ", ID );
	raise ( SIGABRT );

	return nullptr;
}
void PhysicsManager::MoveBonusBoxes( double delta )
{
	auto func = [ = ] ( std::shared_ptr< BonusBox > curr )
	{
		Vector2f direction = curr->GetDirection();

		curr->rect.x += direction.x * delta * curr->GetSpeed();
		curr->rect.y += direction.y * delta * curr->GetSpeed();

		if ( curr->rect.x < 0.0 || ( curr->rect.x + curr->rect.w ) > windowSize.w )
			curr->FlipXDir();

		return curr;
	};

	std::transform( bonusBoxList.begin(), bonusBoxList.end(), bonusBoxList.begin() , func );
}
void PhysicsManager::SetBonusBoxDirection( const std::shared_ptr< BonusBox > &bonusBox, Vector2f dir_ ) const
{
	Vector2f dir = dir_;
	if ( bonusBox->GetOwner()  == Player::Local )
		dir.y = ( dir.y > 0.0 ) ? dir.y : dir.y * -1.0;
	else
		dir.y = ( dir.y < 0.0 ) ? dir.y : dir.y * -1.0;

	bonusBox->SetDirection( dir );
}
BonusType PhysicsManager::GetRandomBonusType() const
{
	int rand = RandomHelper::GenRandomNumber( 1000 );

	if ( rand < 400 )
		return BonusType::BallSplit;
	if ( rand < 650 )
		return BonusType::ExpandPaddle;
	if ( rand < 850 )
		return BonusType::ShrinkPaddle;
	if ( rand < 960 )
		return BonusType::FireBullets;
	if ( rand < 980 )
		return BonusType::ExtraLife;
	else
		return BonusType::Death;
}
// Bullets
// =============================================================================================================
void PhysicsManager::AddBullet( const std::shared_ptr< Bullet > &bullet )
{
	bulletList.push_back( bullet );
}
void PhysicsManager::RemoveBullet( const std::shared_ptr< Bullet >  &bullet )
{
	bulletList.erase( std::find( bulletList.begin(), bulletList.end(), bullet) );
}
std::shared_ptr< Bullet > PhysicsManager::GetBulletWithID( int32_t ID, const Player &owner  )
{
	for ( const auto &p : bulletList )
	{
		if ( ID == p->GetObjectID() && owner == p->GetOwner() )
		{
			return p;
		}
	}

	logger->Log( __FILE__, __LINE__, "Bullet doesn't exist : ", ID );
	raise ( SIGABRT );

	return nullptr;
}
std::shared_ptr< Bullet >  PhysicsManager::CreateBullet( int32_t id, const Player &owner, Vector2f pos )
{
	if ( owner == Player::Local )
		id = ++objectCount;
	std::shared_ptr< Bullet > bullet = std::make_shared< Bullet >( id );

	bullet->SetSpeed( bulletSpeed  );
	bullet->SetPosition( pos );
	bullet->SetOwner( owner );

	bulletList.push_back( bullet );

	return bullet;
}
void PhysicsManager::KillBulletWithID( int32_t id, const Player &owner )
{
	const auto &bullet = GetBulletWithID( id, owner );

	if ( bullet != nullptr )
		bullet->Kill();
}
bool PhysicsManager::DidBulletHitTile( const std::shared_ptr< Bullet > &bullet, const std::shared_ptr< Tile > &tile )
{
	if ( !tile->IsAlive() )
		return false;

	if ( !bullet->HasHitTile( tile->rect ) )
		return false;

	return true;
}
std::shared_ptr< Tile > PhysicsManager::CheckBulletTileIntersections( const std::shared_ptr< Bullet > &bullet )
{
	bool bulletHitTile = false;
	std::shared_ptr< Tile > lowestTile;
	double lowestTileY = 0;

	for ( const auto &tile : tileList )
	{
		if ( !DidBulletHitTile( bullet, tile ) )
			continue;

		bulletHitTile = true;

		if ( tile->rect.y > lowestTileY )
		{
			lowestTile = tile;
			lowestTileY = tile->rect.y;
		}
	}

	return lowestTile;
}
std::vector< std::shared_ptr< Tile > >  PhysicsManager::FindAllTilesOnBulletsPath( const std::shared_ptr< Bullet > &bullet )
{
	std::vector< std::shared_ptr< Tile > > tilesHitByBullet;

	for ( const auto &tile : tileList )
	{
		if ( bullet->WillHitTile( tile->rect ) )
			tilesHitByBullet.push_back( tile );
	}

	return tilesHitByBullet;
}
void PhysicsManager::RespawnBalls( const Player &owner, double ballSpeed )
{
	for ( const auto &ball : ballList )
	{
		if ( ball->GetOwner() == owner )

			ball->SetSpeed ( ballSpeed * ( ( windowSize.h ) / 1080.0 ));
			auto dir = ball->GetDirection();

			if ( (owner == Player::Local && dir.y > 0.0 ) || (owner == Player::Remote && dir.y < 0.0 ) )
			{
				dir.y *= -1.0;
				ball->SetDirection( dir );
			}
	}
}
// Paddles
// =============================================================================================================
void PhysicsManager::SetPaddleData( )
{
	localPaddle->textureType = TextureType::e_Paddle;
	localPaddle->rect.w = 120;
	localPaddle->rect.h = 30;
	localPaddle->rect.x = 400;
	localPaddle->rect.y = windowSize.h - ( localPaddle->rect.h * 1.5 );
	localPaddle->SetScale( scale );

	remotePaddle->textureType = TextureType::e_Paddle;
	remotePaddle->rect.w = 120;
	remotePaddle->rect.h = 30;
	remotePaddle->rect.x = 400;
	remotePaddle->rect.y = remotePaddle->rect.h * 0.5;
	remotePaddle->SetScale( scale );
}
void PhysicsManager::SetLocalPaddlePosition( int32_t x )
{
	localPaddle->rect.x = static_cast< double > ( x ) - ( localPaddle->rect.w / 2 );

	if ( ( localPaddle->rect.x + localPaddle->rect.w ) > windowSize.w )
		localPaddle->rect.x = static_cast< double > ( windowSize.w ) - localPaddle->rect.w;

	if ( localPaddle->rect.x  <= 0  )
		localPaddle->rect.x = 0;

	messageSender.SendPaddlePosMessage( localPaddle->rect.x );
}
bool PhysicsManager::IsTimeForAIMove( std::shared_ptr< Ball > highest ) const
{
	if ( !localPaddle )
	{
		logger->Log( __FILE__, __LINE__, "Local paddle invalid!" );
		raise( SIGABRT );
		return false;
	}

	if ( ballList.size() == 0 )
		return false;

	if ( highest == nullptr )
		return false;

	if ( highest->GetOwner() == Player::Remote )
		return false;

	if ( ( highest->rect.y + highest->rect.h ) <  localPaddle->rect.y  )
		return false;

	return true;
}
void PhysicsManager::AIMove()
{
	const auto &highest = FindHighestBall();

	if ( !IsTimeForAIMove( highest ) )
		return;

	double deadCenter = ( highest->rect.x + highest->rect.w / 2 ) -
		( ( localPaddle->rect.w / 2.0) *
		  RandomHelper::GenRandomNumber( -1.0, 1.0 ) );

	localPaddle->rect.x = deadCenter;
}
// Explosions
// =============================================================================================================
std::vector< Rect > PhysicsManager::GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const
{
	std::vector< std::shared_ptr< Tile > > explodeVec = FindAllExplosiveTilesExcept( explodingTile );

	// The remaning tiles' vects are added if the tile intersects the originating explosions.
	Rect explodeRect( explodingTile->rect );
	explodeRect.DoubleRectSizes();

	std::vector< Rect > explodedTileRects;
	explodedTileRects.push_back( explodeRect );

	bool newExplosion = true;
	while ( explodeVec.size() > 0 && newExplosion )
	{
		newExplosion = false;

		for ( const auto &p : explodeVec )
		{
			if ( RectHelpers::CheckTileIntersection( explodedTileRects, p->rect) )
			{
				Rect r = p->rect;
				r.DoubleRectSizes();
				explodedTileRects.push_back( r );

				p->Kill();
				newExplosion = true;
			}
		}

		// Remove tiles marked as killed
		auto newEnd = std::remove_if( explodeVec.begin(), explodeVec.end(), []( std::shared_ptr< Tile > curr ){ return !curr->IsAlive(); } );
		explodeVec.erase( newEnd, explodeVec.end( ) );
	}

	return explodedTileRects;
}
std::vector< std::shared_ptr< Tile > > PhysicsManager::FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const
{
	std::vector< std::shared_ptr< Tile > > explodingTileVec;

	// A simple lambda to only copy explosive and thoose that are different from explodinTile
	auto copyExplosive = [ explodingTile ]( std::shared_ptr< Tile > tile )
	{
		return  ( tile->GetTileType() == TileType::Explosive ) &&
			( tile != explodingTile );
	};

	// Back inserter because std::copy/std::copy_if expects an iterator that can write to the vector. The ones obtained by std::begin() can't.
	std::copy_if(
			tileList.begin(),
			tileList.end(),
			std::back_inserter( explodingTileVec ),
			copyExplosive
			);

	return explodingTileVec;
}


double PhysicsManager::ResetScale( )
{
	double tempScale = 1.0 / scale;
	scale = 1.0;

	for ( const auto &p : tileList )
	{
		p->rect.x = ( p->rect.x * tempScale ) + ( ( windowSize.w - ( windowSize.w * tempScale ) ) * 0.5 );
		p->rect.y = ( p->rect.y * tempScale ) + ( ( windowSize.h - ( windowSize.h * tempScale ) ) * 0.5 );

		p->SetScale( tempScale );
	}

	for ( const auto &p : ballList )
	{
		p->SetScale( tempScale );
	}
	return scale;
}
void PhysicsManager::ApplyScale( double scale_ )
{
	scale = scale_;

	for ( const auto &p : tileList )
	{
		p->rect.x = static_cast< int32_t > ( ( p->rect.x * scale ) + ( ( windowSize.w - ( windowSize.w * scale ) ) * 0.5 ) );
		p->rect.y = static_cast< int32_t > ( ( p->rect.y * scale ) + ( ( windowSize.h - ( windowSize.h * scale ) ) * 0.5 ) );

		p->ResetScale();
		p->SetScale( scale );
	}
}
void PhysicsManager::KillBallsAndBonusBoxes( const Player &player )
{
	for ( const auto &p : ballList )
	{
		if ( p->GetOwner() == player )
			p->Kill();
	}
	for ( const auto &p : bonusBoxList )
	{
		if ( p->GetOwner() == player )
			p->Kill();
	}
}
void PhysicsManager::SetScale( double scale_ )
{
	scale = scale_;
}
double PhysicsManager::GetScale() const
{
	return scale;
}
void PhysicsManager::SetBulletSpeed( double bulletSpeed_ )
{
	bulletSpeed = bulletSpeed_;
}
void PhysicsManager::SetBonusBoxSpeed( double bonusBoxSpeed_ )
{
	bonusBoxSpeed = bonusBoxSpeed_;
}
void PhysicsManager::SetWindowSize( const SDL_Rect &wSize )
{
	windowSize = wSize;
}
void PhysicsManager::SetPaddles( const std::shared_ptr < Paddle > &localPaddle_, const std::shared_ptr < Paddle > &remotePaddle_ )
{
	localPaddle = localPaddle_;
	remotePaddle = remotePaddle_;
}
void PhysicsManager::Clear()
{
	bulletList.erase( bulletList.begin(), bulletList.end() );
	tileList.erase( tileList.begin(), tileList.end() );
}
void PhysicsManager::UpdateScale()
{
	double left = windowSize.w;
	double right = 0;
	double bottom = 0;
	double top = windowSize.h;
	double width = 0;
	double height = 0;

	for ( const auto &p : tileList )
	{
		if ( height == 0 || width == 0 )
		{
			width = p->rect.w;
			height = p->rect.h;
		}
		if ( p->rect.y > bottom )
			bottom = p->rect.y;

		if ( p->rect.y < top )
			top = p->rect.y;

		if ( p->rect.x < left )
			left = p->rect.x;

		if ( p->rect.x > right )
			right = p->rect.x;
	}

	right += width;
	bottom += height;
	Rect boardSize;
	boardSize.x = left;
	boardSize.y = top;
	boardSize.w = right  - left;
	boardSize.h = bottom - top;

	double minDistToBottom  = windowSize.h * 0.22;
	double scaleeee= (  boardSize.h - ( ( minDistToBottom - top ) * 2 )) / boardSize.h;
	logger->Log( __FILE__, __LINE__, "Board before scaling", boardSize );
	logger->Log( __FILE__, __LINE__, "Scale", scaleeee);

	ApplyScale( scaleeee );
}
