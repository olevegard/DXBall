#include "Ball.h"

	Ball::Ball()
		:	speed( 0.0705 )
		,	dirX( -0.83205 )
		,	dirY(-0.5547 )
		,	stop( false )
	{
		rect.w = 20;
		rect.h = 20;
		Reset();
	}

	Ball::~Ball()
	{
	}

	void Ball::Reset()
	{
		speed = 0.7f;
		dirX = -0.83205;
		dirY =  -0.87;
		rect.x = 300;
		rect.y = 110;
		NormalizeDirection();
	}

	void Ball::NormalizeDirection()
	{
		float length = sqrt( ( dirX * dirX ) + ( dirY * dirY ) );
		dirX /= length;
		dirY /= length;
	}

	void Ball::Update( double tick )
	{
		int deltaMovement = static_cast<int>( tick * speed + 0.5f );
		rect.x += static_cast<int> ( deltaMovement * dirX );
		rect.y += static_cast<int> ( deltaMovement * dirY );
	}

	bool Ball::BoundCheck( const SDL_Rect &boundsRect )
	{
		int left = boundsRect.x;
		int right = boundsRect.x + boundsRect.w;
		int top = boundsRect.y;

		if ( rect.x < left )
		{
			rect.x = left;
			dirX *= -1.0f;
			return true;
		}

		if ( ( rect.x + rect.w ) > right )
		{
			rect.x = ( right - rect.w );
			dirX *= -1.0f;
			return true;
		}

		if ( rect.y < top )
		{
			rect.y = top;
			dirY *= -1.0f;
			return true;
		}

		return false;
	}

	bool Ball::DeathCheck( const SDL_Rect &boundsRect )
	{
		int bottom = boundsRect.y + boundsRect.h;

		if (  ( rect.y + rect.h ) > bottom  )
		{
			Reset();
			dirY *= -1.0f;
			return true;
		}

		return false;
	}

	bool Ball::PaddleCheck( const SDL_Rect &paddleRect )
	{
		int left = paddleRect.x;
		int right = paddleRect.x + paddleRect.w;
		int top = paddleRect.y;

		if ( ( left < ( rect.x + rect.w ) ) && ( right > rect.x ) && ( top < ( rect.y + rect.h  )  ) )
		{
			// Make ball go left of right depending where on the paddle it hits
			int ballMidpoint = rect.x + ( rect.w / 2 );
			int paddleMidpoint = paddleRect.x + ( paddleRect.w / 2 );
			dirX = static_cast<float> ( ballMidpoint - paddleMidpoint ) / 70.0f;
			dirY = -1.0f;
		
			NormalizeDirection();

			speed *= 1.05f;

			return true;
		}

		return false;
	}
