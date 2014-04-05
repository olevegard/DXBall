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
