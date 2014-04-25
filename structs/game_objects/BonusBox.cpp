#include "BonusBox.h"

BonusBox::BonusBox( int32_t objectID_ )
{
	SetObjectID( objectID_ );
	rect.x = 100;
	rect.y = 100;
	rect.w =  30;
	rect.h =  30;

	SetSpeed( 0.15 );
}

void BonusBox::SetOwner( const Player &hitBy )
{
	owner = hitBy;
}

Player BonusBox::GetOwner() const
{
	return owner;
}
void BonusBox::FlipXDir()
{
	direction.x *= -1.0;
}
