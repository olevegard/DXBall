#pragma once

#include <map>

#include "MenuList.h"
#include "ConfigItem.h"

#include "../../enums/ConfigValueType.h"

struct ConfigList : public List
{
	public:

	ConfigList( );
	~ConfigList( )
	{
	}

	void AddConfigItem( std::shared_ptr< ConfigItem > item, const ConfigValueType &type )
	{
		item->SetRectY( GetItemsTop() );
		item->SetRectX( GetItemsLeft() );
		IncrementItemsTop( item->GetTotalHeight() );
		configItems[ type ] = item;
	}

	const std::map< ConfigValueType, std::shared_ptr< ConfigItem > > &GetConfigList() const
	{
		return configItems;
	}
	private:
	void ScrollDown( );
	void ScrollUp( );
	std::map< ConfigValueType, std::shared_ptr< ConfigItem > > configItems;
};


