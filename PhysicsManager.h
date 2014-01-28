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

	PhysicsManager( MessageSender &msgSender )
		:	messageSender( msgSender )
		,	scale( 1.0 )
		,	tileCount( 0 )
	{
	}
	void AddTile( const std::shared_ptr< Tile > &tile )
	{
		tileList.push_back( tile );
	}

	void RemoveTile( const std::shared_ptr< Tile >  &tile )
	{
		--tileCount;
		tileList.erase( std::find( tileList.begin(), tileList.end(), tile) );
	}
	std::shared_ptr< Tile > CreateTile( int16_t xPos, int16_t yPos, const TileType &tileType )
	{
		std::shared_ptr< Tile > tile = std::make_shared< Tile >( tileType, tileCount++ );
		tile->textureType = TextureType::e_Tile;

		tile->rect.x = xPos;
		tile->rect.y = yPos;
		tile->rect.w = 60 * scale;
		tile->rect.h = 20 * scale;

		return tile;
	}
	void AddBall( const std::shared_ptr< Ball > &ball )
	{
		ballList.push_back( ball );
	}
	void RemoveBall( const std::shared_ptr< Ball >  &ball )
	{
		ballList.erase( std::find( ballList.begin(), ballList.end(), ball) );
	}

	void AddBonusBox( const std::shared_ptr< BonusBox > &bb )
	{
		bonusBoxList.push_back( bb );
	}
	void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb )
	{
		bonusBoxList.erase( std::find( bonusBoxList.begin(), bonusBoxList.end(), bb) );
	}

	void AddBullet( const std::shared_ptr< Bullet > &bullet )
	{
		bulletList.push_back( bullet );
	}
	void SetWindowSize( const SDL_Rect &wSize )
	{
		windowSize = wSize;
	}
	std::shared_ptr< Bullet >  FireBullet( int32_t id, const Player &owner, double xPos, double yPos )
	{
		std::shared_ptr< Bullet > bullet = std::make_shared< Bullet >( id );

		bullet->SetSpeed( bulletSpeed  );
		bullet->SetPosition( xPos, yPos );
		bullet->SetOwner( owner );

		bulletList.push_back( bullet );

		return bullet;
	}
	void SetPaddles( std::shared_ptr < Paddle > localPaddle_, std::shared_ptr < Paddle > remotePaddle_ )
	{
		localPaddle = localPaddle_;
		remotePaddle = remotePaddle_;
	}
	void Clear()
	{
		bulletList.clear();
		bonusBoxList.clear();
		tileList.clear();
		ballList.clear();
		tileCount = 0;
	}
	void SetPaddleData( )
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
	void SetLocalPaddlePosition( int32_t x )
	{
		localPaddle->rect.x = static_cast< double > ( x ) - ( localPaddle->rect.w / 2 );

		if ( ( localPaddle->rect.x + localPaddle->rect.w ) > windowSize.w )
			localPaddle->rect.x = static_cast< double > ( windowSize.w ) - localPaddle->rect.w;

		if ( localPaddle->rect.x  <= 0  )
			localPaddle->rect.x = 0;

		messageSender.SendPaddlePosMessage( localPaddle->rect.x );
	}
	void RemoveBullet( const std::shared_ptr< Bullet >  &bullet )
	{
		bulletList.erase( std::find( bulletList.begin(), bulletList.end(), bullet) );
	}
	std::shared_ptr< Ball > GetBallFromID( int32_t ID )
	{
		for ( auto p : ballList )
		{
			if ( ID == p->GetObjectID() )
			{
				return p;
			}
		}

		return nullptr;
	}
	void KillBulletWithID( int32_t id )
	{
		auto bullet = GetBulletFromID( id );

		if ( bullet == nullptr )
		{
			std::cout << "PhysicsManager@" << __LINE__ << " Bullet with ID : " << id << " doesn't exist\n";
			return;
		}
		else
			std::cout << "PhysicsManager@" << __LINE__ << " Deleting bullet with ID : " << id << std::endl;

		bullet->Kill();
	}
	std::shared_ptr< Bullet > GetBulletFromID( int32_t ID )
	{
		for ( auto p : bulletList )
		{
			if ( ID == p->GetObjectID() )
			{
				return p;
			}
		}

		return nullptr;
	}
	std::shared_ptr< Tile > GetTileFromID( int32_t ID )
	{
		for ( auto p : tileList )
		{
			if ( ID == p->GetObjectID() )
				return p;
		}

		return nullptr;
	}
	std::shared_ptr< BonusBox > GetBonusBoxFromID( int32_t ID )
	{
		for ( auto p : bonusBoxList )
		{
			if ( ID == p->GetObjectID() )
			{
				return p;
			}
		}

		return nullptr;
	}
	void UpdateBallSpeed( double localPlayerSpeed, double remotePlayerSpeed )
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
	std::shared_ptr< Ball > FindHighestBall()
	{
		double yMax = 0;
		std::shared_ptr< Ball > highest = nullptr;;
		for ( auto p : ballList )
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
	std::shared_ptr< Tile > FindClosestIntersectingTile( std::shared_ptr< Ball > ball )
	{
		std::shared_ptr< Tile > closestTile;
		double closest = std::numeric_limits< double >::max();
		double current = closest;

		for ( auto p : tileList)
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
	std::vector< Rect > GenereateExplosionRects( const std::shared_ptr< Tile > &explodingTile ) const
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

			for ( auto p : explodeVec )
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
	std::vector< std::shared_ptr< Tile > > FindAllExplosiveTilesExcept( const std::shared_ptr< Tile > &explodingTile ) const
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
	void MoveBonusBoxes( double delta )
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
	bool KillAllTilesWithOwner( const Player &player )
	{
		bool tilesKilled = false;
		for ( auto p : ballList )
		{
			if ( p->GetOwner() == player )
			{
				p->Kill();
				tilesKilled = true;
			}
		}
		return tilesKilled;
	}
	int32_t CountDestroyableTiles()
	{
		auto IsTileDestroyable = []( const std::shared_ptr< Tile > &tile )
		{
			return ( tile->GetTileType() != TileType::Unbreakable );
		};

		return static_cast< int32_t > ( std::count_if( tileList.begin(), tileList.end(), IsTileDestroyable ) );
	}
	double ResetScale( )
	{
		double tempScale = 1.0 / scale;
		scale = 1.0;

		for ( auto& p : tileList )
		{
			p->rect.x = ( p->rect.x * tempScale ) + ( ( windowSize.w - ( windowSize.w * tempScale ) ) * 0.5 );
			p->rect.y = ( p->rect.y * tempScale ) + ( ( windowSize.h - ( windowSize.h * tempScale ) ) * 0.5 );

			p->SetScale( tempScale );
		}

		for ( auto& p : ballList )
		{
			p->SetScale( tempScale );
		}
		return scale;
	}
	void ApplyScale( double scale_ )
	{
		scale = scale_;

		for ( auto& p : tileList )
		{
			p->rect.x = ( p->rect.x * scale ) + ( ( windowSize.w - ( windowSize.w * scale ) ) * 0.5 );
			p->rect.y = ( p->rect.y * scale ) + ( ( windowSize.h - ( windowSize.h * scale ) ) * 0.5 );

			p->ResetScale();
			p->SetScale( scale );
		}

		for ( auto& p : ballList )
		{
			p->ResetScale();
			p->SetScale( scale );
		}
	}
	void KillBallsAndBonusBoxes( const Player &player )
	{
		for ( auto p : ballList )
		{
			if ( p->GetOwner() == player )
				p->Kill();
		}
		for ( auto p : bonusBoxList )
		{
			if ( p->GetOwner() == player )
				p->Kill();
		}
	}
	void SetBonusBoxDirection( std::shared_ptr< BonusBox > bonusBox, Vector2f dir_ ) const
	{
		Vector2f dir = dir_;
		if ( bonusBox->GetOwner()  == Player::Local )
			dir.y = ( dir.y > 0.0 ) ? dir.y : dir.y * -1.0;
		else
			dir.y = ( dir.y < 0.0 ) ? dir.y : dir.y * -1.0;

		bonusBox->SetDirection( dir );
	}
	void SetScale( double scale_ )
	{
		scale = scale_;
	}
	void SetBulletSpeed( double bulletSpeed_ )
	{
		bulletSpeed = bulletSpeed_;
	}

private:
	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
	std::vector< std::shared_ptr< Bullet > > bulletList;

	std::shared_ptr < Paddle > localPaddle;
	std::shared_ptr < Paddle > remotePaddle;

	MessageSender &messageSender;

	SDL_Rect windowSize;
	double scale;

	double bulletSpeed;

	uint32_t tileCount;
};
