#include "Bullet.h"
#include "math/Rect.h"

Bullet::Bullet( int32_t objectID )
{
	rect.w = 10;
	rect.h = 10;
	SetObjectID( objectID );
	SetSpeed( 0.9 );
}
void Bullet::SetOwner( const Player &owner_ )
{
	owner = owner_;
}
Player Bullet::GetOwner() const
{
	return owner;
}
void Bullet::Update( double delta )
{
	oldRect.y = rect.y;

	if ( owner == Player::Local )
		rect.y -= delta * GetSpeed();
	else
		rect.y += delta * GetSpeed();
}
bool Bullet::HasHitTile( const Rect &rectToHit ) const
{
	double tileLeft   = rectToHit.x;
	double tileRight  = rectToHit.x + rectToHit.w;
	double tileBottom = rectToHit.y + rectToHit.h;

	double bulletLeft   = rect.x;
	double bulletRight  = rect.x + rect.w;
	double bulletTop    = rect.y;

	if  ( bulletRight < tileLeft || bulletLeft > tileRight )
		return false;

	return ( bulletTop < tileBottom );
}
bool Bullet::IsOutOfBounds( )
{
	return rect.y < 0;
}
bool Bullet::WillHitTile( const Rect &tileRect  )
{
	double tileLeft = tileRect.x;
	double tileRight = tileLeft + tileRect.w;

	double bulletLeft = rect.x;
	double bulletRight = tileLeft + rect.w;

	if ( bulletLeft > tileRight || bulletRight < tileLeft )
		return false;
	else
		return true;
}
