#pragma once

#include <cstdint>

#include "math/Rect.h"
#include "math/Vector2f.h"

#include "enums/TextureType.h"

struct SDL_Texture;
struct GamePiece
{
	GamePiece();

	Rect rect;
	Rect oldRect;
	Rect originalSize;

	TextureType textureType;

	void Kill()
	{
		isAlive = false;
	}
	bool IsAlive() const
	{
		return isAlive;
	}
	void SetScale( double scale_ )
	{
		scale = scale_;
		rect.w *= scale;
		rect.h *= scale;
	}
	void SetOriginalSize( const SDL_Rect &r );
	void ResetSize( );
	double GetScale( ) const
	{
		return scale;
	}
	void ResetScale()
	{
		//rect.w *= ( 1.0 / scale );
		//rect.h *= ( 1.0 / scale );
		rect.w = 60;
		rect.h = 20;
	}
	int32_t GetObjectID() const
	{
		return objectID;
	}
	void SetObjectID( int32_t objectID_ )
	{
		objectID = objectID_;
	}
	void SetSpeed( double speed_ )
	{
		speed = speed_;
	}
	double GetSpeed() const
	{
		return speed;
	}
	void SetPosition( const Vector2f &pos )
	{
		oldRect.x = rect.x;
		oldRect.y = rect.y;

		rect.x = pos.x;
		rect.y = pos.y;
	}
	Vector2f GetPosition() const
	{
		return Vector2f( rect.x, rect.y );
	}
	void SetDirection( Vector2f dir_ )
	{
		dir = dir_;
	}
	Vector2f GetDirection( ) const
	{
		return dir;
	}
	Vector2f GetDirection_YFlipped( ) const
	{
		return Vector2f( dir.x, dir.y * -1.0 );
	}
	void SetTexture( SDL_Texture* generatedTexture );
	SDL_Texture* GetTexture( ) const;

	protected:
	Vector2f dir;
	private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );

	int32_t objectID;
	bool isAlive;
	double scale;
	double speed;
	SDL_Texture* texture;
};
