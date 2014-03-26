#include "MainMenuItem.h"

    MainMenuItem::MainMenuItem( std::string name, MainMenuItemType mit)
		:	MenuItem( name )
	{
		menuItemType = mit;
	}

    MainMenuItem::~MainMenuItem()
    {
    }


