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

	void Update( double tick );

	bool BoundCheck( const SDL_Rect &boundsRect );
	bool DeathCheck( const SDL_Rect &boundsRect );
	bool PaddleCheck( const Rect &paddleRect );
	bool TileCheck( const Rect &tileRect, bool isSuperBall );
	bool DidBallHitTile( const Rect &tileRect );

	bool CheckTileSphereIntersection( const Rect &tile, const Rect &ball, double &retDistance ) const;

	void SetOwner( Player owner );
	Player GetOwner( ) const;

	void SetRemoteScale( double scale_ );

	Vector2f GetDirection( ) const;
	void SetDirection( const Vector2f &newDir );
	private:

	void NormalizeDirection();

	// Bounds Check Helpers
	//==================================
	bool CheckSideCollisions( const SDL_Rect &boundsRect );
	bool CheckTopBottomCollision( const SDL_Rect &boundsRect );

	// Paddle Hit
	//==================================
	void HandlePaddleHit( const Rect &paddleRect );
	double CalculatePaddleHitPosition( const Rect &paddleRect ) const;
	void CalculateNewBallDirection( double  hitPosition );
	void MoveBallOutOfPaddle( double paddleEdge );

	// Find intersectin side
	// ==================================
	int FindIntersectingSide( const Rect &tileRect );
	bool LineLineIntersectionTestV2(
			const Vector2f &tile1,
			const Vector2f &tile2,
			const Vector2f &ball1,
			const Vector2f &ball2,
			double &ret
	) const;
	bool CheckDotProducts( double dot, double dotTile, double dotBall ) const;
	void ChangeBallDirection( const Side &side );

	Vector2f GetEsimtatedDir( ) const;
	Vector2f Transform( const Vector2f &vec, const Corner &side, const Rect &size ) const;

	double dirX;
	double dirY;

	//unsigned int lastTileHit;

	//bool debugMode;

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
