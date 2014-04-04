#include "PauseMenuItem.h"

PauseMenuItem::PauseMenuItem ( std::string name, PauseMenuItemType pit)
	:	MenuItem( name )
{
	pauseItemType = pit;
}

PauseMenuItem::~PauseMenuItem()
{
}

