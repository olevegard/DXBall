#pragma once

#include "GamePiece.h"

#include <string>

struct SDL_Rect;
struct Vector2f;
enum class Player : int;
struct Ball : GamePiece
{
	Ball();

	virtual ~Ball();

	void Reset();
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

	bool CheckTileSphereIntersection( const Rect &tile, const Rect &ball, double &retDistance ) const;

	private:

	enum class Corner
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};
	
	enum class Side
	{
		Top,
		Bottom,
		Left,
		Right,
		Unknown
	};

	bool CheckTileIntersection( const Rect &tile, const Rect &ball ) const;
	void HandleTileIntersection( const Rect &tile );
	void HandleTileIntersection2( const Rect &tileRect );

	bool LineLineIntersectionTest( const Vector2f &tile1, const Vector2f &tile2, const Vector2f &ball1, const Vector2f &ball2  ) const;


	void HandlePaddleHit( const Rect &paddleRect );
	double CalculatePaddleHitPosition( const Rect &paddleRect ) const;
	void  CalculateNewBallDirection( double  hitPosition );

	void PrintPosition( const Rect &pos, const std::string &tilename ) const;

	// Find intersectin side
	// ==================================
	int FindIntersectingSide( const Rect &tileRect );
	bool LineLineIntersectionTestV2( const Vector2f &tile1, const Vector2f &tile2, const Vector2f &ball1, const Vector2f &ball2, double &ret  ) const;
	bool CheckDotProducts( double dot, double dotTile, double dotBall ) const;

	Vector2f GetEsimtatedDir( );

	Vector2f Transform( const Vector2f &vec, const Corner &side, const Rect &size ) const;

	double speed;

	double dirX;
	double dirY;

	unsigned int lastTileHit;
	bool paddleHitInPrevFrame;

	bool debugMode;

	Player ballOwner;

	Ball( const Ball &other) = delete;

	Ball& operator=( const Ball &other) = delete;

};
