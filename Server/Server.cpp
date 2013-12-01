#include "Server.h"

Server::Server()
	:	gameCount( 0 )

	,	lastHeight( 0 )
	,	screenRect( { 0, 0, 500, 800 } )
{
}
bool Server::InitSDL()
{
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : SLD_Init failed : " << SDL_GetError();
		return false;
	}
	return true;
}
bool Server::CreateWindow()
{
	window = SDL_CreateWindow(
			"DXBall Server",
			screenRect.x,
			screenRect.y,
			screenRect.w,
			screenRect.h,
			SDL_WINDOW_OPENGL
			);

	if ( !window )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : SDL_CreateWindw failed : " << SDL_GetError();
		return false;
	}
	return true;
}
bool Server::CreateRenderer()
{
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

	if ( !renderer )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : SDL_CreateRender failed : " << SDL_GetError();
		return false;
	}

	SDL_RenderSetLogicalSize( renderer, 500, 800 );

	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
	SDL_RenderClear( renderer );
	SDL_RenderPresent( renderer );

	return true;
}
bool Server::InitTTF()
{
	if ( TTF_Init() == -1 )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : TTF_Init failed : " << TTF_GetError();
		return false;
	}

	return true;
}
bool Server::InitFonts()
{

	fontHeader = TTF_OpenFont( "../media/fonts/sketchy.ttf", 40  );//TTF_OpenFont( "~/Programming/DXBall/media/fonts/sketchy.ttf", 50  );

	if ( !fontHeader )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : TTF_OpenFont failed : " << TTF_GetError();
		return false;
	}

	fontSubHeader = TTF_OpenFont( "../media/fonts/sketchy.ttf", 40  );

	if ( !fontSubHeader )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : TTF_OpenFont failed : " << TTF_GetError();
		return false;
	}

	fontGameLine = TTF_OpenFont( "../media/fonts/sketchy.ttf", 25  );

	if ( !fontGameLine )
	{
		std::cout << "Server.cpp@" << __LINE__ << " : TTF_OpenFont failed : " << TTF_GetError();
		return false;
	}

	return true;
}
bool Server::InitHeader()
{
	SDL_Color textColor{ 255, 0, 255, 255 };
	SDL_Surface* surf = TTF_RenderText_Solid( fontHeader, "Server", textColor );
	textureHeader = SDL_CreateTextureFromSurface( renderer, surf );

	rectHeader.w = surf->clip_rect.w;
	rectHeader.h = surf->clip_rect.h;
	rectHeader.x = static_cast< int32_t > ( ( 500 * 0.5 ) - (  rectHeader.w * 0.5 ) );
	rectHeader.y = 10;

	SDL_FreeSurface( surf );

	return true;
}
bool Server::InitSubHeader()
{

	SDL_Color textColor{ 0, 255, 255, 255 };
	SDL_Surface* surf = TTF_RenderText_Solid( fontSubHeader, "Games : ", textColor );
	textureSubHeader = SDL_CreateTextureFromSurface( renderer, surf );

	rectSubHeader.w = surf->clip_rect.w;
	rectSubHeader.h = surf->clip_rect.h;
	rectSubHeader.x = 10;
	rectSubHeader.y = rectPlayerCount.y + rectPlayerCount.h + 10;

	SDL_FreeSurface( surf );

	return true;
}

bool Server::InitPlayerCount( int32_t playerCount )
{
	std::stringstream ss;
	ss << "Players : " << playerCount;
	SDL_Color textColor{ 0, 255, 255, 255 };
	SDL_Surface* surf = TTF_RenderText_Solid( fontSubHeader, ss.str().c_str(), textColor );
	texturePlayerCount = SDL_CreateTextureFromSurface( renderer, surf );

	rectPlayerCount.w = surf->clip_rect.w;
	rectPlayerCount.h = surf->clip_rect.h;
	rectPlayerCount.x = 10;
	rectPlayerCount.y = rectHeader.y + 60;
	lastHeight = rectSubHeader.y + 50;

	SDL_FreeSurface( surf );

	return true;

}
void Server::AddGameLine( const std::string &IP, int32_t port )
{
	std::stringstream ss;
	ss << "IP : " << IP << " | port : " << port;
	SDL_Color textColor{ 0, 255, 255, 255 };
	SDL_Surface* surf = TTF_RenderText_Solid( fontGameLine, ss.str().c_str(), textColor );
	SDL_Texture* textureGameLine = SDL_CreateTextureFromSurface( renderer, surf );

	SDL_Rect rectGameLine;

	rectGameLine.w = surf->clip_rect.w;
	rectGameLine.h = surf->clip_rect.h;
	rectGameLine.x = rectSubHeader.x + 20;
	rectGameLine.y = lastHeight;
	lastHeight += 30;

	SDL_FreeSurface( surf );

	rectsGameLine.push_back( rectGameLine );
	texturesGameLine.push_back( textureGameLine );
}
void Server::RepositionGameLines()
{
	lastHeight = rectSubHeader.y + 50;
	for ( auto &p : rectsGameLine )
	{
		p.y = lastHeight;
		lastHeight += 30;
	}
}
bool Server::Init()
{
	if ( !InitSDL() )
		return false;

	if ( !CreateWindow() )
		return false;

	if ( !CreateRenderer() )
		return false;

	if ( !InitTTF() )
		return false;

	if ( !InitFonts() )
		return false;

	if ( !InitPlayerCount() )
		return false;

	if ( !InitHeader() )
		return false;

	if ( !InitSubHeader() )
		return false;

	if ( !InitNet( "127.0.0.1", 3113 ) )
		return false;


	return true;
}
bool Server::InitNet( std::string ip, uint16_t port )
{
	if ( SDLNet_Init() < 0 )
	{
		std::cout << "Server.cpp@" << __LINE__ << " Failed to init SDLNet : " << SDLNet_GetError() << std::endl;
		return false;
	}

	if ( !connection.Init( ip, port, true ) )
	{
		std::cout << "Server.cpp@" << __LINE__ << " Failed to init connection : " << SDLNet_GetError() << std::endl;
		return false;
	}

	std::cout << "Net initialized" << std::endl;

	return true;
}
void Server::Run()
{
	bool run = true;
	std::cout << "Run..\n";
	while ( run )
	{
		SDL_Event event;

		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_q:
					case SDLK_ESCAPE:
						run = false;
				}
			}
			else if ( event.type == SDL_QUIT )
				run = false;
		}
		bool newConnection = connection.Update();
		int32_t countConnections = connection.GetActiveConnectionsCount();
		for ( int i = 0; i < countConnections ; ++i )
			UpdateNetwork( i );
		if ( newConnection )
			InitPlayerCount( countConnections );

		SDL_RenderClear( renderer );
		SDL_RenderCopy( renderer, textureHeader, nullptr, &rectHeader);
		SDL_RenderCopy( renderer, texturePlayerCount, nullptr, &rectPlayerCount);
		SDL_RenderCopy( renderer, textureSubHeader, nullptr, &rectSubHeader);
		for ( uint32_t i = 0; i < texturesGameLine.size() ; ++i )
			SDL_RenderCopy( renderer, texturesGameLine[i] , nullptr, &rectsGameLine[i]);

		SDL_RenderPresent( renderer );
	}
	std::cout << "Main loop done\n";
}
void Server::UpdateNetwork( int connectionNo )
{
	std::string str = connection.ReadMessages( connectionNo );

	std::stringstream ss;
	ss << str;

	if ( str == "" )
		return;
	std::cout << "Received : " << str << std::endl;

	TCPMessage msg;
	while ( ss >> msg )
	{
		if ( msg.GetType() == MessageType::NewGame )
		{
			AddGameLine( msg.GetIPAdress(), msg.GetPort() );
			GameInfo game;
			game.Set( msg.GetIPAdress(), msg.GetPort() );
			game.SetGameID( gameCount );

			std::cout << "Server.cpp@" << __LINE__ << " Adding game with ID : " << gameCount << std::endl;
			gameList.push_back( game );
			++gameCount;
		}
		else if ( msg.GetType() == MessageType::GameJoined )
		{
			RecieveGameJoinedMessage( msg );

			SendMessageToAll( msg );
		}
		else if ( msg.GetType() == MessageType::EndGame )
		{
			RecieveGameEndMessage( msg );
			SendMessageToAll( msg );
		}
		else if ( msg.GetType() == MessageType::GetGameList )
		{
			SendGameList( connectionNo );
		}
	}
}

void Server::RecieveGameEndMessage(const TCPMessage &msg )
{
	std::cout << "Delete message received for : " << msg.GetObjectID() << std::endl;
	std::string deleteIP  = msg.GetIPAdress();
	int32_t deletePort  = msg.GetPort();
	int32_t deletedGames = 0;

	for ( uint32_t i = 0; i < gameList.size() ; ++i )
	{
		if ( gameList[i].GetGameID() == msg.GetObjectID() )
		{
			++deletedGames;

			gameList.erase( gameList.begin() + i );
			rectsGameLine.erase( rectsGameLine.begin() + i );
			texturesGameLine.erase( texturesGameLine.begin() + i );
		}
	}

	if ( deletedGames == 0 )
		std::cout << "Server.cpp@" << __LINE__
			<< " no games deleted for Game ID : " << msg.GetObjectID() << std::endl;
	else if ( deletedGames > 1 )
		std::cout << "Server.cpp@" << __LINE__
			<< " more than 1 games deleted for Game ID : " << msg.GetObjectID() << std::endl;
	else
		std::cout << "Game deleted!\n";

	RepositionGameLines();

}
void Server::SendGameList( int32_t connectionNo )
{
	std::cout << "Get game list. From : " << connectionNo << std::endl;
	std::cout << "Size of game list : " << gameList.size() << std::endl;

	for ( const auto &p : gameList )
	{
		TCPMessage msg;
		msg.SetMessageType( MessageType::NewGame );
		msg.SetIPAdress( p.GetIP() );
		msg.SetPort( static_cast< uint16_t > ( p.GetPort() ) );
		msg.SetObjectID( p.GetGameID() );
		std::stringstream ss;
		ss << msg;
		connection.Send( ss.str(), connectionNo );
		std::cout << "Sending : " << ss.str() << std::endl;
	}

	std::cout << "========= DONE SENDING ==========\n";
}
void Server::RecieveGameJoinedMessage( const TCPMessage &msg )
{
	int32_t deleteID = msg.GetObjectID();
	std::cout << "Delete message received for : " << deleteID << std::endl;
	int32_t deletedGames = 0;

	for ( uint32_t i = 0; i < gameList.size() ; ++i )
	{
		if ( gameList[i].GetGameID() == deleteID )
		{
			++deletedGames;

			gameList.erase( gameList.begin() + i );
			rectsGameLine.erase( rectsGameLine.begin() + i );
			texturesGameLine.erase( texturesGameLine.begin() + i );
		}
	}

	if ( deletedGames == 0 )
	{
		std::cout << "Server.cpp@" << __LINE__
			<< " no games deleted for Game ID : " << deleteID
			<< std::endl;

	}
	else if ( deletedGames > 1 )
	{
		std::cout << "Server.cpp@" << __LINE__
			<< " more than 1 games deleted for Game ID : "
			<< deleteID << std::endl;
	}

	std::cout << "Game deleted!\n";
	RepositionGameLines();
}
void Server::SendMessageToAll( const TCPMessage &msg )
{
	int32_t countConnections = connection.GetActiveConnectionsCount();
	for ( int i = 0; i < countConnections ; ++i )
	{
		std::stringstream ss;
		ss << msg;
		connection.Send( ss.str(), i );
	}
}
