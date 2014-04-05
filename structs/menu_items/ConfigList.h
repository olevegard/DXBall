#pragma once

#include <map>

#include "MenuList.h"
#include "ConfigItem.h"

#include "../../enums/ConfigValueType.h"

struct ConfigList : public List
{
	public:

	ConfigList( );
	~ConfigList( );

	void AddConfigItem( std::shared_ptr< ConfigItem > item, const ConfigValueType &type );

	const std::map< ConfigValueType, std::shared_ptr< ConfigItem > > &GetConfigList() const;
	void Set( uint32_t value, ConfigValueType type );

	private:
	void ScrollDown( );
	void ScrollUp( );
	std::map< ConfigValueType, std::shared_ptr< ConfigItem > > configItems;
};


