#include "Bullet.h"
#include "math/Rect.h"

Bullet::Bullet( int32_t objectID )
{
	rect.w = 5;
	rect.h = 5;
	SetObjectID( objectID );
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
	if ( owner == Player::Local )
		rect.y -= delta * 0.15;
	else
		rect.y += delta * 0.15;
}
void Bullet::SetPosition( double x, double y )
{
	rect.x = x;
	rect.y = y;
}
