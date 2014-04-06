#include "ConfigList.h"

ConfigList::ConfigList( )
{

}
ConfigList::~ConfigList( )
{
}
void ConfigList::AddConfigItem( std::shared_ptr< ConfigItem > item, const ConfigValueType &type )
{
	item->SetRectY( GetItemsTop() );
	item->SetRectX( GetItemsLeft() );

	IncrementItemsTop( item->GetTotalHeight() );

	configItems[ type ] = item;
}
const std::map< ConfigValueType, std::shared_ptr< ConfigItem > > &ConfigList::GetConfigList() const
{
	return configItems;
}
void ConfigList::Set( uint32_t value, ConfigValueType type )
{
	configItems[ type ]->SetValue( value );
}
void ConfigList::ConfigList::ScrollDown( )
{
	for ( auto &item : configItems )
		item.second->MoveUp( 10 );
}
void ConfigList::ScrollUp( )
{
	for ( auto &item : configItems )
		item.second->MoveDown( 10 );
}
int32_t ConfigList::FindTopItem() const
{
	int32_t topItem = 100000;
	for ( const auto &item : configItems )
	{
		if ( item.second->GetTop() < topItem )
			topItem = item.second->GetTop();
	}

	return topItem;
}
int32_t ConfigList::FindBottomItem() const
{
	int32_t bottomItem = -1;
	for ( const auto &item : configItems )
	{
		if ( item.second->GetBottom() > bottomItem )
			bottomItem = item.second->GetBottom();
	}

	return bottomItem;
}
