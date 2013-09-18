#include "BonusBox.h"

BonusBox::BonusBox()
	:	effectName( "Life" )
{
	rect.x = 100;
	rect.y = 100;
	rect.w =  40;
	rect.h =  40;//84
}

std::string BonusBox::GetName() const
{
	return effectName;
}

void BonusBox::SetTexture( SDL_Texture* generatedTexture )
{
	texture = generatedTexture;
}
SDL_Texture* BonusBox::GetTexture( ) const
{
	return texture;
}

void BonusBox::SetOwner( const Player &hitBy )
{
	owner = hitBy;
}

Player BonusBox::GetOwner() const
{
	return owner;
}

void BonusBox::SetDirection( const Vector2f &dir )
{
	direction = dir;
}
Vector2f BonusBox::GetDirection( ) const
{
	return direction;
}

void BonusBox::FlipXDir()
{
	direction.x *= -1.0;
}
