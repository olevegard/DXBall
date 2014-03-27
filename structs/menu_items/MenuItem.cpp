#include "MenuItem.h"

#include <iostream>

MenuItem::MenuItem( std::string name)
    :	itemName( name )
    ,	itemRect( {0,0,0,0} )
    ,	texture( nullptr )
    ,	isSelected( false )
    ,	hasChanged( true )
{
}
MenuItem::MenuItem( const MenuItem &item )
    :   itemName( item.GetName() )
    ,   itemRect( item.GetRect() )
    ,   texture( item.GetTexture() )
{

}
MenuItem::~MenuItem()
{
}
void MenuItem::SetRect( SDL_Rect r )
{
    itemRect = r;
}
void MenuItem::SetRectXY( int x, int y )
{
    itemRect.x = x;
    itemRect.y = y;
}
void MenuItem::SetRectX( int x )
{
    itemRect.x = x;
}
void MenuItem::SetRectY( int y )
{
    itemRect.y = y;
}
void MenuItem::MoveDown( int32_t y )
{
    std::cout << "Move down...\n";
    itemRect.y += y;
}
int MenuItem::GetRectX( ) const
{
    return itemRect.x;
}
int MenuItem::GetRectY( ) const
{
    return itemRect.y;
}
int MenuItem::GetRectW( ) const
{
    return itemRect.w;
}
int MenuItem::GetRectH( ) const
{
    return itemRect.h;
}
int MenuItem::GetEndX() const
{
    return itemRect.x + itemRect.w;
}
SDL_Rect MenuItem::GetRect() const
{
    return itemRect;
}
const SDL_Rect* MenuItem::GetRectPtr() const
{
    return &itemRect;
}
void MenuItem::SetTexture( SDL_Texture* text )
{
    SDL_DestroyTexture( texture );
    texture = text;
}
SDL_Texture* MenuItem::GetTexture( ) const
{
    return texture;
}
bool MenuItem::IsSelected() const
{
    return isSelected;
}
void MenuItem::SetSelcted( bool selected )
{
    if ( isSelected != selected )
        hasChanged = true;

    isSelected = selected;
}
bool MenuItem::HasValidTexture() const
{
    return texture != nullptr;
}
bool MenuItem::HasUnderlineChanged()
{
    if ( hasChanged )
    {
        hasChanged = false;
        return true;
    }
    else
        return false;
}
void MenuItem::SetName( std::string str )
{
    itemName = str;
}
std::string MenuItem::GetName( ) const
{
    return itemName;
}
