#include "ConfigList.h"

ConfigList::ConfigList( )
{

}
void ConfigList::ScrollDown( )
{
	for ( auto &item : configItems )
		item.second->MoveUp( 10 );
}
void ConfigList::ScrollUp( )
{
	for ( auto &item : configItems )
		item.second->MoveDown( 10 );
}
