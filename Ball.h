#pragma once

#include "GamePiece.h"

#include "enums/Player.h"

struct SDL_Rect;
struct Vector2f;
enum class Side : int;
enum class Corner : int;
enum class Player : int;
struct Ball : GamePiece
{
	Ball( const SDL_Rect &windowSize, const Player &owner, int32_t ID );

	virtual ~Ball();

	void Reset(const SDL_Rect &windowSize );
	void NormalizeDirection();

	void Update( double tick );

	bool BoundCheck( const SDL_Rect &boundsRect );

	bool DeathCheck( const SDL_Rect &boundsRect );

	bool PaddleCheck( const Rect &paddleRect );

	bool TileCheck( const Rect &paddleRect, unsigned int tileID );

	void SetOwner( Player owner )
	{
		ballOwner = owner;
	}

	Player GetOwner( ) const
	{
		return ballOwner;
	}
	void SetRemoteScale( double scale_ )
	{
		SetSpeed( GetSpeed() * scale_ );
	}
	bool CheckTileSphereIntersection( const Rect &tile, const Rect &ball, double &retDistance ) const;


	Vector2f GetDirection( ) const;
	void SetDirection( const Vector2f &newDir );
	private:

	bool CheckTileIntersection( const Rect &tile, const Rect &ball ) const;
	void HandleTileIntersection( const Rect &tile );
	void HandleTileIntersection2( const Rect &tileRect );

	bool LineLineIntersectionTest( const Vector2f &tile1, const Vector2f &tile2, const Vector2f &ball1, const Vector2f &ball2  ) const;

	void HandlePaddleHit( const Rect &paddleRect );
	double CalculatePaddleHitPosition( const Rect &paddleRect ) const;
	void  CalculateNewBallDirection( double  hitPosition );

	// Find intersectin side
	// ==================================
	int FindIntersectingSide( const Rect &tileRect );
	bool LineLineIntersectionTestV2( const Vector2f &tile1, const Vector2f &tile2, const Vector2f &ball1, const Vector2f &ball2, double &ret  ) const;
	bool CheckDotProducts( double dot, double dotTile, double dotBall ) const;

	Vector2f GetEsimtatedDir( ) const;

	Vector2f Transform( const Vector2f &vec, const Corner &side, const Rect &size ) const;

	//double speed;

	double dirX;
	double dirY;

	unsigned int lastTileHit;
	bool paddleHitInPrevFrame;

	bool debugMode;

	Player ballOwner;

	Ball( const Ball &other) = delete;
	Ball& operator=( const Ball &other) = delete;
};

inline bool operator==( const Ball &ball1, const Ball &ball2)
{
	return ball1.GetObjectID() == ball2.GetObjectID();
}
inline bool operator==( const Ball &ball, int32_t ID)
{
	return ball.GetObjectID() == ID;
}
inline bool operator==( int32_t ID, const Ball &ball )
{
	return ball.GetObjectID() == ID;
}
inline bool operator!=( const Ball &ball1, const Ball &ball2)
{
	return !(ball1 == ball2);
}
