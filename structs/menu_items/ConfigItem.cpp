#include "ConfigItem.h"

#include <iostream>

	ConfigItem::ConfigItem ( std::string name)
	:	MenuItem( name )
	,	isBool( false )
	,	boolValue( false )
{
}

ConfigItem::~ConfigItem()
{
}
void ConfigItem::SetValueTexture( SDL_Texture* texture )
{
	valueTexture = texture;
}
void ConfigItem::SetValueRect( SDL_Rect r )
{
	valueRect = r;
	GeneratePlussMinus();
}
SDL_Texture* ConfigItem::GetValueTexture( ) const
{
	return valueTexture;
}
SDL_Rect ConfigItem::GetValueRect( ) const
{
	return valueRect;
}
const SDL_Rect* ConfigItem::GetValueRectPtr( ) const
{
	return &valueRect;
}
const SDL_Rect* ConfigItem::GetPlussRectPtr( ) const
{
	return &plussRect;
}
const SDL_Rect* ConfigItem::GetMinusRectPtr( ) const
{
	return &minusRect;
}
SDL_Rect ConfigItem::GetPlussRect( ) const
{
	return plussRect;
}
SDL_Rect ConfigItem::GetMinusRect( ) const
{
	return minusRect;
}
void ConfigItem::GeneratePlussMinus()
{
	SDL_Rect square = valueRect;
	square.w = 15;
	square.h = square.w;


	square.x += ( valueRect.w /2 ) - ( square.w / 2 );
	square.y -= square.h - 5;

	plussRect = square;

	minusRect = square;
	minusRect.y = valueRect.y + valueRect.h - 5;
}
void ConfigItem::SetValue( uint32_t value_ )
{
	if ( value == value_ )
		return;

	value = value_;
	hasChanged = true;
}
void ConfigItem::SetValue( bool value_ )
{
	//if ( value == boolValue ) return;

	boolValue = value_;
	hasChanged = true;
}
uint32_t ConfigItem::GetValue()
{
	hasChanged = false;
	return value;
}
bool ConfigItem::HasChanged()
{
	return hasChanged;
}
void ConfigItem::MoveDown( int32_t amount )
{
	valueRect.y += amount;
	plussRect.y += amount;
	minusRect.y += amount;
	MenuItem::MoveDown( amount );
}
void ConfigItem::MoveUp( int32_t amount )
{
	valueRect.y -= amount;
	plussRect.y -= amount;
	minusRect.y -= amount;
	MenuItem::MoveUp( amount );
}
void ConfigItem::MoveRight( int32_t amount )
{
	valueRect.x += amount;
	plussRect.x += amount;
	minusRect.x += amount;
	MenuItem::MoveRight( amount );
}
void ConfigItem::SetRectX( int32_t X )
{
	int32_t diff = X - GetLeft();
	if ( diff > 0 )
		MoveRight( diff );
}
void ConfigItem::SetRectY( int32_t y )
{
	int32_t diff = y - plussRect.y;
	if ( diff > 0 )
		MoveDown( diff );
}
int32_t ConfigItem::GetLeft() const
{
	return MenuItem::GetLeft();
}
int32_t ConfigItem::GetRight() const
{
	return valueRect.x + valueRect.w;
}int32_t ConfigItem::GetTop() const
{
	return plussRect.y;
}
int32_t ConfigItem::GetBottom() const
{
	return minusRect.y + minusRect.h;
}
int32_t ConfigItem::GetTotalHeight()
{
	return ( minusRect.y + minusRect.h ) - plussRect.y;
}
